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
	int ipos = Str.find_first_of(Delim), ilastpos = 0;
	while (ipos != std::string::npos)
	{
		std::string strSub = Str.substr(ilastpos, ipos - ilastpos);
		IntVec.push_back(atoi(strSub.c_str()));
		ilastpos = ipos + 1;
		ipos = Str.find_first_of(Delim, ilastpos);
	}
	std::string strSub = Str.substr(ilastpos);
	IntVec.push_back(atoi(strSub.c_str()));
	return IntVec.size();
}

inline int GetParamFromString(std::string Str, std::vector<float>& FloatVec, char Delim = ',')
{
	int ipos = Str.find_first_of(Delim), ilastpos = 0;
	while (ipos != std::string::npos)
	{
		std::string strSub = Str.substr(ilastpos, ipos - ilastpos);
		FloatVec.push_back(atof(strSub.c_str()));
		ilastpos = ipos + 1;
		ipos = Str.find_first_of(Delim, ilastpos);
	}
	std::string strSub = Str.substr(ilastpos);
	FloatVec.push_back(atof(strSub.c_str()));
	return FloatVec.size();
}

inline int GetParamFromString(std::string Str, std::vector<u32>& uiIntVec, char Delim = ',')
{
	int ipos = Str.find_first_of(Delim), ilastpos = 0;
	while (ipos != std::string::npos)
	{
		std::string strSub = Str.substr(ilastpos, ipos - ilastpos);
		uiIntVec.push_back(strtoul(strSub.c_str(), NULL, 10));
		ilastpos = ipos + 1;
		ipos = Str.find_first_of(Delim, ilastpos);
	}
	std::string strSub = Str.substr(ilastpos);
	uiIntVec.push_back(strtoul(strSub.c_str(), NULL, 10));
	return uiIntVec.size();
}

inline int GetParamFromString(std::string Str, std::vector<std::string>& StringVec, char Delim = ',')
{
	int ipos = 0, ilastpos = 0, iquotpos = std::string::npos;
	if (ilastpos >= Str.size())
		return 0;
	if (Str.at(ilastpos) == '\"')
	{
		iquotpos = Str.find_first_of('\"', ilastpos + 1);
		if (iquotpos != std::string::npos)
			ipos = Str.find_first_of(Delim, iquotpos);
		else
			ipos = Str.find_first_of(Delim, ilastpos);
	}
	else
		ipos = Str.find_first_of(Delim, ilastpos);

	while (ipos != std::string::npos)
	{
		std::string strSub = Str.substr(ilastpos, ipos - ilastpos);
		if (iquotpos != std::string::npos)
			strSub = strSub.substr(1, strSub.size() - 2);
		StringVec.push_back(strSub);

		ilastpos = ipos + 1;
		if (ilastpos >= Str.size())
			break;
		if (Str.at(ilastpos) == '\"')
		{
			iquotpos = Str.find_first_of('\"', ilastpos + 1);
			if (iquotpos != std::string::npos)
				ipos = Str.find_first_of(Delim, iquotpos);
			else
				ipos = Str.find_first_of(Delim, ilastpos);
		}
		else
		{
			iquotpos = std::string::npos;
			ipos = Str.find_first_of(Delim, ilastpos);
		}
	}

	std::string strSub = Str.substr(ilastpos);
	if (iquotpos != std::string::npos)
		strSub = strSub.substr(1, strSub.size() - 2);
	StringVec.push_back(strSub);

	return StringVec.size();
}

template<typename T>
inline int GetParamFromStringEx(std::string Str, std::vector<T>& vec, char Delim = ',')
{
	int ipos = Str.find_first_of(Delim), ilastpos = 0;
	while (ipos != std::string::npos)
	{
		std::string strSub = Str.substr(ilastpos, ipos - ilastpos);
		std::stringstream stream;
		stream << strSub;
		T v;
		stream >> v;
		vec.push_back(v);
		ilastpos = ipos + 1;
		ipos = Str.find_first_of(Delim, ilastpos);
	}
	if (ilastpos < Str.size())
	{
		std::string strSub = Str.substr(ilastpos);
		std::stringstream stream;
		stream << strSub;
		T v;
		stream >> v;
		vec.push_back(v);
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

template<typename T>
std::string SetParamToSimpleArea(std::vector<T>& vecParams, char left = '[', char right = ']', char Delim = ';')
{
	std::string strRet;
	strRet += left;
	std::vector<T>::iterator iter = vecParams.begin();
	for (; iter != vecParams.end(); iter++)
	{
		if (iter != vecParams.begin())
			strRet += Delim;
		std::stringstream stream;
		stream << *iter;
		strRet += stream.str().c_str();
	}
	strRet += right;
	return strRet;
}








};