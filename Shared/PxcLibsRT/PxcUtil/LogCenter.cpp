#include "LogCenter.h"
#include "DateTime.h"
#include "FileManage.h"
#include "StringTools.h"

#define STR_LOG_FILE_HEAD "==========日志文件 : %s=========="

namespace PxcUtil
{

CLogCenter::CLogCenter()
{
	m_pWritingStream = NULL;
}

bool CLogCenter::Init(int iFileID, const char* szFileName)
{
	if (!m_mapFiles.empty())
		return false;

	bool bCreate = true;
	if (FileManage::IsFileExist(StringTools::StrToWstr(szFileName).c_str()))
		bCreate = false;

	m_mapFiles[iFileID] = std::ofstream();
	std::ofstream& stream = m_mapFiles[iFileID];
	stream.open(szFileName, std::ios_base::out | std::ios_base::app);
	if (stream.bad())
	{
		std::cout << "日志初始化失败!" << std::endl;
		m_mapFiles.erase(iFileID);
		return false;
	}

	if (bCreate)
	{
		char szHead[MAX_PATH * 2];
		sprintf_s(szHead, STR_LOG_FILE_HEAD, szFileName);
		stream << szHead << std::endl;
	}
	return true;
}

void CLogCenter::Release()
{
	std::map<int, std::ofstream>::iterator iter = m_mapFiles.begin();
	for (; iter != m_mapFiles.end(); iter++)
	{
		iter->second.close();
	}
	m_mapFiles.clear();
}

bool CLogCenter::AddFile(int iFileID, const char* szFileName)
{
	if (m_mapFiles.find(iFileID) != m_mapFiles.end())
		return false;

	bool bCreate = true;
	if (FileManage::IsFileExist(StringTools::StrToWstr(szFileName).c_str()))
		bCreate = false;

	m_mapFiles[iFileID] = std::ofstream();
	std::ofstream& stream = m_mapFiles[iFileID];
	stream.open(szFileName, std::ios_base::out | std::ios_base::app);
	if (stream.bad())
	{
		std::cout << "日志文件添加失败!" << std::endl;
		m_mapFiles.erase(iFileID);
		return false;
	}

	if (bCreate)
	{
		char szHead[MAX_PATH * 2];
		sprintf_s(szHead, STR_LOG_FILE_HEAD, szFileName);
		stream << szHead << std::endl;
	}
	return true;
}

void CLogCenter::RemoveFile(int iFileID)
{
	std::map<int, std::ofstream>::iterator iter = m_mapFiles.find(iFileID);
	if (iter != m_mapFiles.end())
	{
		iter->second.close();
		m_mapFiles.erase(iter);
	}
}

CLogCenter& CLogCenter::WriteStream(int iFileID, ELogLevel eLevel)
{
	CRI_SEC(m_lock)
#ifndef _DEBUG
	if (eLevel == ELogLevel_Debug)
	{
		m_pWritingStream = NULL;
		return *this;
	}
#endif
	std::map<int, std::ofstream>::iterator iter = m_mapFiles.find(iFileID);
	if (iter != m_mapFiles.end())
	{
		m_pWritingStream = &iter->second;
		*m_pWritingStream << std::endl;

		DateTimeInfo timeInfo;
		if (DateTime::InformDateTime(DateTime::GetDateTime(), timeInfo))
			*m_pWritingStream << "[" << timeInfo.date << " " << timeInfo.iHour << ":" << timeInfo.iMin << ":" << timeInfo.iSec << "] ";

		switch (eLevel)
		{
		case ELogLevel_Debug:
			*m_pWritingStream << "DEBUG: "; break;
		case ELogLevel_Warning:
			*m_pWritingStream << "WARNING: "; break;
		case ELogLevel_Assert:
			*m_pWritingStream << "ASSERT: "; break;
		case ELogLevel_Error:
			*m_pWritingStream << "ERROR: "; break;
		}
	}
	else
		m_pWritingStream = NULL;
	return *this;
}

CLogCenter& CLogCenter::WriteAssert(int iFileID, const char* szAsFileName, int iAsLine, const char* szExp, bool bAssert)
{
	CRI_SEC(m_lock)
	WriteStream(iFileID, ELogLevel_Assert) << "(File:" << szAsFileName << " Line:" << iAsLine << " Expression:" << szExp;
	if (m_pWritingStream)
	{
		if (!bAssert)
		{
			*m_pWritingStream << " False) ";
			std::cout << "日志 断言为否! 文件:" << szAsFileName << " 行号:" << iAsLine << " 表达式:" << szExp << std::endl;
		}
		else
			*m_pWritingStream << " True) ";
	}
	return *this;
}

CLogCenter& CLogCenter::WriteStreamDesc(int iFileID, ELogLevel eLevel, const char* szDesc)
{
	CRI_SEC(m_lock)
	WriteStream(iFileID, eLevel) << szDesc << " ";
	if (m_pWritingStream && (int)eLevel > (int)ELogLevel_Info)
	{
		switch (eLevel)
		{
		case ELogLevel_Warning:
			std::cout << "日志 警告: "; break;
		case ELogLevel_Assert:
			std::cout << "日志 非正式断言! "; break;
		case ELogLevel_Error:
			std::cout << "日志 错误! "; break;
		}
		std::cout << szDesc << std::endl;
	}
	return *this;
}

}