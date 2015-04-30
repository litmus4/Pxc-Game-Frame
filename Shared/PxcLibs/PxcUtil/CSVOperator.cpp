#include "CSVOperator.h"
#include "zPackEx.h"

namespace PxcUtil
{

//////////////////////////////////////////////////////////////////////////
//CSV operator

CCSVOperator::CCSVOperator(const char* path)
{
    LoadCSV(path);
}

bool CCSVOperator::LoadCSV(const char* path)
{
	zp::IReadFile* pZFile = NULL;
	FILE* pfile = NULL;
	if (zPackFOpen(path, &pZFile) == EzPOpen_SimplePath)
		pfile = fopen(path, "r");

    if (pZFile || pfile)
    {
		char* filebuffer = NULL;
		char cnl = '\n';
		int inl = 1;
		if (pZFile)
		{
			filebuffer = new char[pZFile->size()];
			pZFile->read((zp::u8*)filebuffer, pZFile->size());
			cnl = '\r';
			inl++;
		}
		else
		{
			fseek(pfile,0,SEEK_END);
			u32 dwsize = ftell(pfile);
			rewind(pfile);

			filebuffer = new char[dwsize];
			fread(filebuffer, 1, dwsize, pfile);
		}

        std::map<u32, std::string> StringMap;
        char* pBegin = filebuffer;
        char* pEnd = strchr(filebuffer, cnl);
        u32 uiIndex = 1;
        while (pEnd != NULL)
        {
            std::string strbuff;
            strbuff.insert(0, pBegin, pEnd-pBegin);
            if (!strbuff.empty())
            {
                StringMap[uiIndex] = strbuff;
            }
            pBegin = pEnd + inl;
            pEnd = strchr(pEnd + inl, cnl);
            ++uiIndex;
        }
        delete[] filebuffer;

        std::map<u32, std::string>::iterator iter = StringMap.begin();
        for (; iter != StringMap.end(); ++iter)
        {
            std::vector<std::string> StringVec;
            std::map<u32, std::string> l_StringMap;
            StringParser::GetParamFromString(iter->second, StringVec);
            for (int i = 0; i < StringVec.size(); ++i)
            {
                l_StringMap[i+1] = StringVec.at(i);
            }
            m_StringKeyMap[iter->first] = l_StringMap;
        }
		if (pZFile)
			zPackFClose(pZFile);
		else
			fclose(pfile);
        m_CSVName = path;
        return true;
    }

    return false;
}


bool CCSVOperator::GetInt(u32 uiLine, u32 uiRow, int& iValue)
{
    std::string* pKey = GetString(uiLine, uiRow);
    if (pKey)
    {
        iValue = atoi(pKey->c_str());
        return true;
    }
    else
    {
        return false;
    }
}

bool CCSVOperator::GetFloat(u32 uiLine, u32 uiRow, float& fValue)
{
    std::string* pKey = GetString(uiLine, uiRow);
    if (pKey)
    {
        fValue = atof(pKey->c_str());
        return true;
    }
    else
    {
        return false;
    }
}

std::string* CCSVOperator::GetString(u32 uiLine, u32 uiRow)
{
    std::map<u32, std::map<u32, std::string> >::iterator iterLine = m_StringKeyMap.find(uiLine);
    if (iterLine != m_StringKeyMap.end())
    {
        std::map<u32, std::string>& rStringMap = iterLine->second;
        std::map<u32, std::string>::iterator iterRow = rStringMap.find(uiRow);
        if (iterRow != rStringMap.end())
        {
            return &iterRow->second;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

bool CCSVOperator::SetNumber(u32 uiLine, u32 uiRow, int iValue)
{
    std::string* pKey = GetString(uiLine, uiRow);
    if (pKey)
    {
        char buffer[100];
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%d", iValue);
        pKey->clear();
        *pKey = buffer;
        return true;
    }
    else
    {
        return false;
    }
}

bool CCSVOperator::SetNumber(u32 uiLine, u32 uiRow, float fValue)
{
    std::string* pKey = GetString(uiLine, uiRow);
    if (pKey)
    {
        char buffer[100];
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%d", fValue);
        pKey->clear();
        *pKey = buffer;
        return true;
    }
    else
    {
        return false;
    }
}

bool CCSVOperator::SetString(u32 uiLine, u32 uiRow, const char* pStr)
{
    std::string* pKey = GetString(uiLine, uiRow);
    if (pKey)
    {
        pKey->clear();
        *pKey = pStr;
        return true;
    }
    else
    {
        return false;
    }
}

bool CCSVOperator::SaveCSV(const char* path)
{
    if (path != NULL)
    {
        m_CSVName = path;
    }

    FILE* pfile = fopen(m_CSVName.c_str(), "w");
    if (pfile)
    {
        std::map<u32, std::map<u32, std::string> >::iterator iter = m_StringKeyMap.begin();
        for (; iter != m_StringKeyMap.end(); ++iter)
        {
            std::map<u32, std::string>& rStringMap = iter->second;
            std::map<u32, std::string>::iterator it = rStringMap.begin();
            for (; it != rStringMap.end(); ++it)
            {
				std::string key;
				if (it != rStringMap.begin())
					key += ',';
				if (it->second.find_first_of(',') != std::string::npos)
				{
					std::string strSec = "\"" + it->second + "\"";
					key += strSec;
				}
				else
					key += it->second;
                fwrite(key.c_str(), 1, key.size(), pfile);
            }
            char Delim = '\n';
            fwrite(&Delim, 1, 1, pfile);
        }
        fclose(pfile);
    }
    else
    {
        return false;
    }

    return true;
}

bool CCSVOperator::IsNoName()
{
	return m_CSVName.empty();
}

}