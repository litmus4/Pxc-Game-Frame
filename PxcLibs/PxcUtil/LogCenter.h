#pragma once
#include "Singleton.h"
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

	CLogCenter& WriteInStream(int iFileID, ELogLevel eLevel, bool bAssert = true);

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
};

}