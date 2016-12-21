#pragma once
#include <ctime>
#include <iostream>

namespace PxcUtil
{

class ExactTime
{
public:
	enum ELevel_Type
	{
		ELevel_Second = 0,
		ELevel_Milli,
		ELevel_Micro,
	};

public:
	static void Init();
	static double GetSysTick();
	static unsigned long long GetPerfTime(ELevel_Type eLevel = ELevel_Second);
	static float GetFloatTime(ELevel_Type eLevel = ELevel_Second, bool bPerf = false);

private:
	static long long s_lFrequency;
};

//========================================================================= 
// 作者   : quarryman
// 邮箱   : quarrying@qq.com
// 链接   : http://blog.csdn.net/quarryman
// 文件   : Date.h
// 日期   : 2013年01月04日
// 更新   : 2013年01月31日
//=========================================================================

class Date
{
private:
	int y,m,d;
public:
	Date();
	explicit Date(int yy,int mm=1,int dd=1);
	Date(const tm& t);
	Date(const Date& other);

	void setDate(int yy,int mm=1,int dd=1);
	void setDate(const tm& t);
	void setDateAsToday();
	//yy年mm月第nn个星期ww
	void setDateAsWeekday(int yy,int mm,int n,int w);
	//yy年第nn个星期ww
	void setDateAsWeekday(int yy,int n,int w);

	int getYear() { return y; }
	int getMonth() { return m;}
	int getDay() { return d; }
	bool isLeapYear() const;
	bool isValidDate() const;
	int getWeekday() const;
	int getDaysOfMonth() const;
	int getDaysOfYear() const;
	int getDaysPassedOfMonth() const;
	int getDaysLeftOfMonth() const;
	int getDaysPassedOfYear() const;
	int getDaysLeftOfYear() const;

	//关系运算符重载
	friend bool operator==(const Date& lhs,const Date& rhs);
	friend bool operator!=(const Date& lhs,const Date& rhs);
	friend bool operator>(const Date& lhs,const Date& rhs);
	friend bool operator>=(const Date& lhs,const Date& rhs);
	friend bool operator<(const Date& lhs,const Date& rhs);
	friend bool operator<=(const Date& lhs,const Date& rhs);
	//日期运算
	Date& addYears(int n);
	Date& addMonths(int n);
	Date& addWeeks(int n);
	Date& addDays(int n);
	//算术运算符重载
	Date& operator++();
	Date operator++(int);
	Date& operator--();
	Date operator--(int);
	friend Date operator+(const Date& a,const int& n);
	friend Date operator+(const int& n,const Date& a);
	friend Date operator-(const Date& a,const int& n);
	friend int operator-(const Date& a,const Date& b);
	//赋值运算符重载
	Date& operator =(const Date& rhs);
	Date& operator +=(const int& n);
	Date& operator -=(const int& n);
	//int operator -=(const Date& rhs);

	friend std::ostream& operator<<(std::ostream &os,const Date &rhs);
};

struct DateTimeInfo
{
	DateTimeInfo()
	{
		Reset();
	}
	void Reset()
	{
		date.setDate(1900);
		iHour = iMin = iSec = 0;
	}
	Date date;
	int iHour;
	int iMin;
	int iSec;
};

namespace DateTime
{

long long GetDateTime();
bool InformDateTime(long long lDateTime, DateTimeInfo& outInfo);
long long IntegrateDateTime(DateTimeInfo& info);

}


}