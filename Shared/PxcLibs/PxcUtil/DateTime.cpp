#include "DateTime.h"
#include <windows.h>
#include <complex>
#include <cmath>
#include <cassert>
#include <algorithm>

namespace PxcUtil
{

long long ExactTime::s_lFrequency = 0;

void ExactTime::Init()
{
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	s_lFrequency = li.QuadPart;
}

double ExactTime::GetSysTick()
{
	if (s_lFrequency == 0)
		return 0;
	return (1.0 / s_lFrequency);
}

unsigned long long ExactTime::GetTime(ELevel_Type eLevel)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return (unsigned long long)((double)li.QuadPart / s_lFrequency * pow(1000.0, (int)eLevel));
}

//====================================================================
// 作者   : quarryman
// 邮箱   : quarrying@qq.com
// 链接   : http://blog.csdn.net/quarryman
// 文件   : Date.cpp
// 日期   : 2013年01月04日
// 更新   : 2013年01月31日
//====================================================================

bool isLeapYear(int year)
{
	return (year % 4 == 0 && year % 100 != 0) 
		|| year % 400 == 0;
}

int getWeekday(int y, int m, int d)
{
	if (m==1 || m==2)
	{
		m+=12;
		y--;
	}
	return (d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7+1;
}

int getDaysOfMonth(int year,int month)
{
	int days=0;
	switch(month)
	{
	case 1: case 3: case 5: case 7:
	case 8: case 10: case 12:
		days=31; break;
	case 4: case 6: case 9: case 11:
		days=30; break;
	case 2:
		days=28+isLeapYear(year);
		break;
	default:
		break;
	}
	return days;
}

int getDaysOfYear(int year)
{
	return 365+isLeapYear(year);
}

int getDaysPassedOfMonth(int year,int month,int day)
{
	return day;
}

int getDaysLeftOfMonth(int year,int month,int day)
{
	return getDaysOfMonth(year,month)-getDaysPassedOfMonth(year,month,day);
}

int getDaysPassedOfYear(int year,int month,int day)
{
	int days=0;
	for(int i=1;i<month;i++)
	{
		days+=getDaysOfMonth(year,i);
	}
	days+=day;

	return days;
}

int getDaysLeftOfYear(int year,int month,int day)
{
	return getDaysOfYear(year)-getDaysPassedOfYear(year,month,day);
}

bool isValidDate(int year, int month, int day)
{
	//不存在公元0年，公元元年为公元1年
	if(year == 0)
	{
		return false;
	}
	if(month<=0 || month>12)
	{
		return false;
	}
	else if(day <=0 || day > getDaysOfMonth(year,month))
	{
		return false;
	}
	return true;
}

int mod_floor(int a,int b)
{
	return a-floor((double)a/b)*b;
}

Date::Date()
{
	//y=m=d=1;
	// 2013-01-31 修改
	const time_t timer=time(0);
	struct tm *now=localtime(&timer);
	y=now->tm_year+1900;
	m=now->tm_mon+1;
	d=now->tm_mday;
}

Date::Date(int yy,int mm,int dd)
{
	if(PxcUtil::isValidDate(yy,mm,dd))
	{
		y=yy;m=mm;d=dd;
	}
	else
	{
		std::cout<<"error date!"<<std::endl;
	}
}

Date::Date(const tm& t)
{
	y=t.tm_year+1900;
	m=t.tm_mon+1;
	d=t.tm_mday;
}

Date::Date(const Date& other)
{
	y=other.y;m=other.m;d=other.d;
}

void Date::setDate(int yy,int mm,int dd)
{
	if(PxcUtil::isValidDate(yy,mm,dd))
	{
		y=yy;m=mm;d=dd;
	}
	else
	{
		std::cout<<"error date!"<<std::endl;
	}
}

void Date::setDate(const tm& t)
{
	y=t.tm_year+1900;
	m=t.tm_mon+1;
	d=t.tm_mday;
}

void Date::setDateAsToday()
{
	const time_t timer=time(0);
	struct tm *now=localtime(&timer);
	y=now->tm_year+1900;
	m=now->tm_mon+1;
	d=now->tm_mday;
}

void Date::setDateAsWeekday(int yy,int mm,int nn,int ww)
{
	int d;
	setDate(yy,mm,1);
	if(nn>0)
	{
		int w=getWeekday();
		if(ww>=w)
		{
			d=(nn-1)*7+ww-w;
		}
		else
		{
			d=(nn-1)*7+7+ww-w;
		}
	}
	else
	{
		setDate(yy,mm,getDaysOfMonth());
		int w=getWeekday();
		if(ww<=w)
		{
			d=(nn+1)*7+ww-w;
		}
		else
		{
			d=(nn+1)*7-7+ww-w;
		}
	}
	addDays(d);
}

void Date::setDateAsWeekday(int yy,int nn,int ww)
{
	int d;
	if(nn>0)
	{
		setDate(yy,1,1);
		int w=getWeekday();
		if(ww>=w)
		{
			d=(nn-1)*7+ww-w;
		}
		else
		{
			d=(nn-1)*7+7+ww-w;
		}
	}
	else
	{
		setDate(yy,12,31);
		int w=getWeekday();
		if(ww<=w)
		{
			d=(nn+1)*7+ww-w;
		}
		else
		{
			d=(nn+1)*7-7+ww-w;
		}
	}
	addDays(d);
}

bool Date::isLeapYear()  const
{
	return PxcUtil::isLeapYear(y);
}

bool Date::isValidDate() const
{
	return PxcUtil::isValidDate(y,m,d);
}

int Date::getWeekday() const
{
	return PxcUtil::getWeekday(y,m,d);
}

int Date::getDaysOfMonth() const
{
	return PxcUtil::getDaysOfMonth(y,m);
}

int Date::getDaysOfYear() const
{
	return PxcUtil::getDaysOfYear(y);
}

int Date::getDaysPassedOfMonth() const
{
	return PxcUtil::getDaysPassedOfMonth(y,m,d);
}

int Date::getDaysLeftOfMonth() const
{
	return PxcUtil::getDaysLeftOfMonth(y,m,d);
}

int Date::getDaysPassedOfYear() const
{
	return PxcUtil::getDaysPassedOfYear(y,m,d);
}

int Date::getDaysLeftOfYear() const
{
	return PxcUtil::getDaysLeftOfYear(y,m,d);
}

//关系运算符重载
bool operator==(const Date& lhs,const Date& rhs)
{
	return lhs.y==rhs.y && lhs.m==rhs.m && lhs.d==rhs.d;
}

bool operator!=(const Date& lhs,const Date& rhs)
{
	return !(lhs==rhs);
}

bool operator<(const Date& lhs,const Date& rhs)
{
	if(lhs.y!=rhs.y)
	{
		return lhs.y<rhs.y;
	}
	else
	{
		if(lhs.m!=rhs.m)
		{
			return lhs.m<rhs.m;
		}
		else
		{
			if(lhs.d!=rhs.d)
			{
				return lhs.d<rhs.d;
			}
			else
			{
				return false;
			}
		}
	}
}

bool operator<=(const Date& lhs,const Date& rhs)
{
	return !(rhs<lhs);
}

bool operator>(const Date& lhs,const Date& rhs)
{
	return rhs<lhs;
}

bool operator>=(const Date& lhs,const Date& rhs)
{
	return !(lhs<rhs);
}

Date& Date::addYears(int n)
{
	y+=n;
	if(2==m && 29==d)
	{
		d=28+isLeapYear();
	}
	//不存在公元0年，公元元年为公元1年
	if(0==y)
	{
		y=n>0?1:-1;
	}
	return *this;
}

Date& Date::addMonths(int n)
{
	y+=floor((m+n-1)/12.0);
	m=mod_floor(m+n-1,12)+1;
	if(d>getDaysOfMonth())
	{
		d=getDaysOfMonth();
	}
	//不存在公元0年，公元元年为公元1年
	if(0==y)
	{
		y=n>0?1:-1;
	}
	return *this;
}

Date& Date::addWeeks(int n)
{
	return addDays(n*7);
}

Date& Date::addDays(int n)
{
	if(n>0)
	{
		if(n<=getDaysLeftOfMonth())
		{
			d+=n;
		}
		else
		{
			n-=getDaysLeftOfMonth()+1;
			addMonths(1); 
			d=1;
			addDays(n);
		}
	}
	else
	{
		if(abs(n)<getDaysPassedOfMonth())
		{
			d+=n;
		}
		else
		{
			n+=getDaysPassedOfMonth();
			addMonths(-1); 
			d=getDaysOfMonth();
			addDays(n);
		}
	}
	return *this;
}

//算术运算符重载
Date& Date::operator++()
{
	if(++d==getDaysOfMonth()+1)
	{
		d=1;
		if(++m==13)
		{
			m=1;
			++y;
		}
	}
	return *this;
}

Date Date::operator++(int)
{
	Date temp=*this;
	++*this;
	return temp;
}

Date& Date::operator--()
{
	if(--d==0)
	{
		--m;
		d=getDaysOfMonth();
		if(m==0)
		{
			m=12;
			--y;
		}
	}
	return *this;
}

Date Date::operator--(int)
{
	Date temp=*this;
	--*this;
	return temp;
}

Date operator+(const Date& a,const int& n)
{
	Date date(a);
	date.addDays(n);
	return date;
}

Date operator+(const int& n,const Date& a)
{
	Date date(a);
	date.addDays(n);
	return date;
}

Date operator-(const Date& a,const int& n)
{
	Date date(a);
	date.addDays(-n);
	return date;
}

int operator-(const Date& a,const Date& b)
{
	int ret=0;
	if(a!=b)
	{
		if(a<b)
		{
			for(int i=a.y;i<b.y;++i)
			{
				ret+=getDaysOfYear(i);
			}
			ret=-ret+a.getDaysPassedOfYear()-b.getDaysPassedOfYear();
		}
		else
		{
			for(int i=b.y;i<a.y;++i)
			{
				ret+=getDaysOfYear(i);
			}
			ret=ret+a.getDaysPassedOfYear()-b.getDaysPassedOfYear();
		}
	}
	else
	{
		ret=a.getDaysPassedOfYear()-b.getDaysPassedOfYear();
	}
	return ret;
}

Date& Date::operator =(const Date& rhs)
{
	y=rhs.y;
	m=rhs.m;
	d=rhs.d;
	return *this;
}

Date& Date::operator +=(const int& n)
{
	addDays(n);
	return *this;
}

Date& Date::operator -=(const int& n)
{
	addDays(-n);
	return *this;
}

std::ostream& operator<<(std::ostream &os,const Date &rhs)
{
	os<<rhs.y<<"-"<<rhs.m<<"-"<<rhs.d;
	return os;
}


namespace DateTime
{

long long GetDateTime()
{
	return (long long)time(NULL);
}

bool InformDateTime(long long lDateTime, DateTimeInfo& outInfo)
{
	if (lDateTime < 0)
		return false;

	struct tm* pTime = localtime((time_t*)&lDateTime);
	outInfo.date.setDate(*pTime);
	outInfo.iHour = pTime->tm_hour;
	outInfo.iMin = pTime->tm_min;
	outInfo.iSec = pTime->tm_sec;
	return true;
}

long long IntegrateDateTime(DateTimeInfo& info)
{
	struct tm sTime;
	sTime.tm_year = info.date.getYear() - 1900;
	sTime.tm_mon = info.date.getMonth() - 1;
	sTime.tm_mday = info.date.getDay();
	sTime.tm_hour = info.iHour;
	sTime.tm_min = info.iMin;
	sTime.tm_sec = info.iSec;
	return (long long)mktime(&sTime);
}

}


}