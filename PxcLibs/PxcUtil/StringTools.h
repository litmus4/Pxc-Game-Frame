#pragma once
#include <string>

namespace PxcUtil
{


namespace StringTools
{

std::wstring StrToWstr(const std::string& str);
std::string WstrToStr(const std::wstring& wstr);

template<typename T>
inline std::string BasicToStr(const T& basic)
{
	std::stringstream stream;
	stream << basic;
	return stream.str().c_str();
}

template<typename T>
inline T StrToBasic(const std::string& str)
{
	T basic = 0;
	std::stringstream stream;
	stream << str;
	stream >> basic;
	return basic;
}

template<typename T>
inline std::wstring BasicToWstr(const T& basic)
{
	std::wstringstream stream;
	stream << basic;
	return stream.str().c_str();
}

template<typename T>
inline T WstrToBasic(const std::wstring& wstr)
{
	T basic = 0;
	std::wstringstream stream;
	stream << wstr;
	stream >> basic;
	return basic;
}

std::string Format(const char* szForm, ...);
std::wstring Format(const wchar_t* wszForm, ...);
std::string Trim(const std::string& str);
std::wstring Trim(const std::wstring& wstr);

}


}