#pragma once
#include "Singleton.h"
#include "Scattered.h"
#include <map>
#include <fstream>

namespace PxcUtil
{

class CLogCenter : public Singleton<CLogCenter>
{
public:
	enum ELogLevel
	{
		ELogLevel_Debug = 0,
		ELogLevel_Info,
		ELogLevel_Warning,
		ELogLevel_Assert,
		ELogLevel_Error,
	};

	DECLARE_SINGLETON_CONSTRUCTOR(CLogCenter)
public:
	bool Init(int iFileID, const char* szFileName);
	void Release();
	bool AddFile(int iFileID, const char* szFileName);
	void RemoveFile(int iFileID);

	CLogCenter& WriteStream(int iFileID, ELogLevel eLevel);
	CLogCenter& WriteAssert(int iFileID, const char* szAsFileName, int iAsLine, const char* szExp, bool bAssert);
	CLogCenter& WriteStreamDesc(int iFileID, ELogLevel eLevel, const char* szDesc);

	template<typename T>
	CLogCenter& operator << (const T value)
	{
		if (m_pWritingStream)
			*m_pWritingStream << value;
		return *this;
	}

private:
	std::map<int, std::ofstream> m_mapFiles;
	std::ofstream* m_pWritingStream;
	Lock m_lock;
};

#define PXCU_LOGINST PxcUtil::CLogCenter::GetInstance()
#define PXCU_LOGLV(levelWord) PxcUtil::CLogCenter::ELogLevel_##levelWord

#define PXCU_LOG(fileId) PXCU_LOGINST->WriteStream(fileId, PXCU_LOGLV(Info))
#define PXCU_LOG_DEBUG(fileId) PXCU_LOGINST->WriteStream(fileId, PXCU_LOGLV(Debug))
#define PXCU_LOG_WARNING(fileId, desc) PXCU_LOGINST->WriteStreamDesc(fileId, PXCU_LOGLV(Warning), desc)
#define PXCU_LOG_ASSERT(fileId, expr) PXCU_LOGINST->WriteAssert(fileId, __FILE__, __LINE__, #expr, (bool)(expr))
#define PXCU_LOG_ERROR(fileId, desc) PXCU_LOGINST->WriteStreamDesc(fileId, PXCU_LOGLV(Error), desc)

}