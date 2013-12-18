#include "LogCenter.h"
#include "DateTime.h"
#include "Scattered.h"

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

	m_mapFiles[iFileID] = std::ofstream();
	std::ofstream& stream = m_mapFiles[iFileID];
	stream.open(szFileName, std::ios_base::out | std::ios_base::app);
	if (!stream)
	{
		m_mapFiles.erase(iFileID);
		return false;
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

	m_mapFiles[iFileID] = std::ofstream();
	std::ofstream& stream = m_mapFiles[iFileID];
	stream.open(szFileName, std::ios_base::out | std::ios_base::app);
	if (!stream)
	{
		m_mapFiles.erase(iFileID);
		return false;
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

CLogCenter& CLogCenter::WriteInStream(int iFileID, ELogLevel eLevel, bool bAssert)
{
	std::map<int, std::ofstream>::iterator iter = m_mapFiles.find(iFileID);
	if (iter != m_mapFiles.end())
	{
		m_pWritingStream = &iter->second;
		*m_pWritingStream << std::endl;//TODO
		//switch (eLevel)
		//{
		//	//
		//}
	}
	return *this;
}

}