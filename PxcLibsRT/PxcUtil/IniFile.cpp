/*
 * IniFile.cpp
 *
 *  Created on: 2008-3-16
 *      Author: jingpeng
 *      Email: tingsking@gmail.com
 */
#include "IniFile.h"
#include <algorithm>    //find() function
#include <string>
#include <fstream>
#ifdef _WIN32
 #ifndef __MINGW32__
  #define snprintf _snprintf
 #endif
#endif

using namespace std;

namespace PxcUtil
{

IniFile::IniFile()
{
    m_bDirty = false;
}

IniFile::~IniFile()
{
}

bool IniFile::Load(const char *szFile)
{
    m_bDirty = false;   //init flag
    m_contents.clear(); //clear storage
    SetPath(szFile);

    string line;

    //open the INI file for reading
    fstream iniFile(szFile, ios::in/*|ios::nocreate*/);    
    if(!iniFile.is_open())
        return false;

    //read from file line by line
    bool bDone = false;
    char szBuffer[4000];

    while(!bDone)
    {
        szBuffer[0] = '\0';
        iniFile.getline(szBuffer, sizeof(szBuffer));
        line = szBuffer;

        //is that a section in the buffer
        if('[' == line[0])
        {
            int nPos = line.find_first_of(']');
            if(-1 != nPos)
            {
                IniSection section;
                section.m_name = line.substr(1, nPos-1).c_str();

                //add new section to the end of storage
                m_contents.push_back(section);
            }
        }
        else
        {
            //else is the line "key=value"
            int nPos = line.find_first_of('=');

            if(nPos > 0)    //there must be space for key name
            {
                //add new key to the last section in the storage
                IniKey key;
                key.m_name  = line.substr(0, nPos).c_str();
                key.m_value = line.substr(nPos+1, line.size()-nPos-1).c_str();

                int size = m_contents.size();
                if(size > 0)
                {
                    m_contents[size-1].m_Keys.push_back(key);
                }
            }

        }

        //check for exit
        bDone = iniFile.eof() || iniFile.bad() || iniFile.fail();
    }

    return true;
}

bool IniFile::Save()
{
    m_bDirty = false;   //reset flag

	//ensure directory exists
//	string strDir = GetParentDir(m_strPath.c_str());
//    EnsureNotTerminated(strDir);
//    EnsureDirExists(strDir);    //TOFIX check success

    //open the INI file for writing
    fstream iniFile(m_strPath.c_str(), ios::out|ios::trunc);
    if(!iniFile.is_open())
        return false;

    char szBuffer[4000];

    int size = m_contents.size();
    for(int i=0; i<size; i++)
    {
        //write line with section name
        snprintf(szBuffer, sizeof(szBuffer), "[%s]\n", m_contents[i].m_name.c_str());
        iniFile.write(szBuffer, strlen(szBuffer));

        int count = m_contents[i].m_Keys.size();
        for(int j=0; j<count; j++)
        {
            //write "key = value"
            snprintf(szBuffer, sizeof(szBuffer), "%s=%s\n",
                m_contents[i].m_Keys[j].m_name.c_str(),
                m_contents[i].m_Keys[j].m_value.c_str());
            iniFile.write(szBuffer, strlen(szBuffer));
        }
        
        iniFile.write("\n", 1);
    }

    return true;
}

bool IniFile::ClearAll()
{
    m_bDirty = true;
    m_contents.clear();
    return true;
}

bool IniFile::SectionExists(const char *szSection)
{
    IniSectionIterator It;
    return FindSection(It, szSection);
}

bool IniFile::KeyExists(const char *szSection, const char *szKey)
{
    IniKeyIterator It;
    return FindKey(It, szSection, szKey);
}

bool IniFile::GetValue(const char *szSection, const char *szKey, string &value, const char *szDefault)
{
    IniKeyIterator It;
    if(FindKey(It, szSection, szKey)){
        value = It->m_value;
        return true;
    }

    value = szDefault;
    return false;
}

bool IniFile::GetValue(const char *szSection, const char *szKey, int  &nValue, int nDefault)
{
    IniKeyIterator It;
    if(FindKey(It, szSection, szKey)){
        nValue = atoi(It->m_value.c_str());
        return true;
    }

    nValue = nDefault;
    return false;
}

bool IniFile::GetValue(const char *szSection, const char *szKey, unsigned short &nValue, int nDefault)
{
    IniKeyIterator It;
    if(FindKey(It, szSection, szKey)){
        nValue = atoi(It->m_value.c_str());
        return true;
    }

    nValue = nDefault;
    return false;
}

bool IniFile::GetValue(const char *szSection, const char *szKey, bool &bValue, int nDefault)
{
    IniKeyIterator It;
    if(FindKey(It, szSection, szKey)){
        bValue = atoi(It->m_value.c_str()) > 0;
        return true;
    }

    bValue = nDefault > 0;
    return false;
}

bool IniFile::SetValue(const char *szSection, const char *szKey, const int nValue)
{
    char szNumber[30] = "";
    snprintf(szNumber, sizeof(szNumber), "%d", nValue);

    return SetValue(szSection, szKey, szNumber);
}

bool IniFile::SetValue(const char *szSection, const char *szKey, const char *szVal)
{
    m_bDirty = true;    //content changes

    IniKeyIterator It;
    if(FindKey(It, szSection, szKey))
    {
        It->m_value = szVal;    //update existing key
        return true;
    }

    if(AddSection(szSection))   //ensure section exists
    {
        IniSectionIterator SecIt;
        if(FindSection(SecIt, szSection))
        {
            IniKey key;
            key.m_name  = szKey;
            key.m_value = szVal;
            
            SecIt->m_Keys.push_back(key);
            return true;
        }
    }

    return false;
}

bool IniFile::AddSection(const char *szSection)
{
    m_bDirty = true;    //content changes

    IniSectionIterator It;

    if(!FindSection(It, szSection))
    {
        IniSection section;
        section.m_name = szSection;

        m_contents.push_back(section);
        return true;
    }

    return true;    //section already exists
}

bool IniFile::RemoveSection(const char *szSection)
{
    IniSectionIterator It;

    if(FindSection(It, szSection))
        m_contents.erase(It);

    return true;
}

bool IniFile::FindSection(IniSectionIterator &It, const char *szSection)
{
    IniSection section;
    section.m_name = szSection;

    It = find(m_contents.begin(), m_contents.end(), section);

    return (It != m_contents.end());
}

bool IniFile::FindKey(IniKeyIterator &It, const char *szSection, const char *szKey)
{
    IniSectionIterator SecIt;

    if(FindSection(SecIt, szSection)) //section exists
    {
        IniKey key;
        key.m_name = szKey;

        It = find(SecIt->m_Keys.begin(), SecIt->m_Keys.end(), key);

        return (It != SecIt->m_Keys.end());
    }

    return false;
}

vector<string> IniFile::GetSections()
{
	vector<string> strVector;
	for(vector<IniSection>::iterator itor = m_contents.begin();itor!=m_contents.end();++itor)
	{
		IniSection* sec = (IniSection*)(&(*itor));
		strVector.push_back(sec->m_name);
	}
	return strVector;
}

}