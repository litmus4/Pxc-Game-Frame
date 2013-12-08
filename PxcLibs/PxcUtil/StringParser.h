#pragma once
#include <process.h>
#include <Windows.h>
#include <map>
#include <vector>
#include <queue>
#include <set>
#include <string>
#include <list>
#include <sstream>

typedef char                    i8;
typedef unsigned char           u8;
typedef short                  i16;
typedef unsigned short          u16;
typedef long int                i32;
typedef unsigned long           u32;



namespace StringParser{







//从分隔符中获得数据
inline int GetParamFromString(std::string Str, std::vector<i32>& IntVec, char Delim = ',')
{
    char* p = strtok((char*)Str.c_str(), &Delim); 
    while (p)
    {
        IntVec.push_back(atoi(p));
        p = strtok(NULL, &Delim); 
    }
    return IntVec.size();
}

inline int GetParamFromString(std::string Str, std::vector<float>& FloatVec, char Delim = ',')
{
    char* p = strtok((char*)Str.c_str(), &Delim); 
    while (p)
    {
        FloatVec.push_back(atof(p));
        p = strtok(NULL, &Delim); 
    }
    return FloatVec.size();
}

inline int GetParamFromString(std::string Str, std::vector<u32>& uiIntVec, char Delim = ',')
{
    char* p = strtok((char*)Str.c_str(), &Delim); 
    while (p)
    {
        uiIntVec.push_back(strtoul(p, NULL, 10));
        p = strtok(NULL, &Delim); 
    }
    return uiIntVec.size();
}

inline int GetParamFromString(std::string Str, std::vector<std::string>& StringVec, char Delim = ',')
{
    char* p = strtok((char*)Str.c_str(), &Delim); 
    while (p)
    {
        std::string buffer = p;
        StringVec.push_back(buffer);
        p = strtok(NULL, &Delim); 
    }
    return StringVec.size();
}

template<typename T>
inline int GetParamFromStringEx(std::string Str, std::vector<T>& vec, char Delim = ',')
{
	char* p = strtok((char*)Str.c_str(), &Delim); 
	while (p)
	{
		std::stringstream stream;
		stream << p;
		T v;
		stream >> v;
		vec.push_back(v);
		p = strtok(NULL, &Delim); 
	}
	return vec.size();
}

//以左右符号得到括号中的数据ex:[3.1415;0.125][1000;9999]
template<typename T>
int GetParamFromArea(std::string Str, std::vector<std::vector<T> >& IntVec, char left = '[', char right = ']', char Delim = ';')
{
    char* pTarget = (char*)Str.c_str();
    for (;;)
    {
        char* pLeft = strchr(pTarget, left);
        char* pRight = strchr(pTarget, right);
        if (pLeft && pRight)
        {
            std::string strbuff;
            strbuff.insert(0, ++pLeft, pRight-pLeft);

            std::vector<T> Intbuff;
            if (GetParamFromStringEx(strbuff, Intbuff, Delim))
            {
                IntVec.push_back(Intbuff);
            }
            pTarget = ++pRight;
        }
        else
        {
            break;
        }
    }
    return IntVec.size();
}









};