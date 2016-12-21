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
// ����   : quarryman
// ����   : quarrying@qq.com
// ����   : http://blog.csdn.net/quarryman
// �ļ�   : Date.h
// ����   : 2013��01��04��
// ����   : 2013��01��31��
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
	//yy��mm�µ�nn������ww
	void setDateAsWeekday(int yy,int mm,int n,int w);
	//yy���nn������ww
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

	//��ϵ���������
	friend bool operator==(const Date& lhs,const Date& rhs);
	friend bool operator!=(const Date& lhs,const Date& rhs);
	friend bool operator>(const Date& lhs,const Date& rhs);
	friend bool operator>=(const Date& lhs,const Date& rhs);
	friend bool operator<(const Date& lhs,const Date& rhs);
	friend bool operator<=(const Date& lhs,const Date& rhs);
	//��������
	Date& addYears(int n);
	Date& addMonths(int n);
	Date& addWeeks(int n);
	Date& addDays(int n);
	//�������������
	Date& operator++();
	Date operator++(int);
	Date& operator--();
	Date operator--(int);
	friend Date operator+(const Date& a,const int& n);
	friend Date operator+(const int& n,const Date& a);
	friend Date operator-(const Date& a,const int& n);
	friend int operator-(const Date& a,const Date& b);
	//��ֵ���������
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