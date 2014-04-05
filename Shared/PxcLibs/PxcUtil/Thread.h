#ifndef __THREAD_H__
#define __THREAD_H__

#include <string>

#include   <windows.h>
#include   <process.h>

namespace PxcUtil
{

class Runnable
{
public:
	virtual ~Runnable() {};
	virtual void Run() = 0;
};

class CThread : public Runnable
{
private:
	explicit CThread(const CThread & rhs);

public:
	CThread();
	CThread(Runnable * pRunnable);
	CThread(const char * ThreadName, Runnable * pRunnable = NULL);
	CThread(std::string ThreadName, Runnable * pRunnable = NULL);
	~CThread(void);

	/**
	  开始运行线程
	  @arg bSuspend 开始运行时是否挂起
	**/
	bool Start(bool bSuspend = false);

	/**
	  运行的线程函数，可以使用派生类重写此函数
	**/
	virtual void Run();

	/**
	  当前执行此函数线程等待线程结束
	  @arg timeout 等待超时时间，如果为负数，等待无限时长
	**/
	void Join(int timeout = -1);
	/**
	  恢复挂起的线程
	**/
	void Resume();
	/**
	  挂起线程
	**/
	void Suspend();
	/**
	  终止线程的执行
	**/
	bool Terminate(unsigned long ExitCode);

	unsigned int GetThreadID();
	std::string GetThreadName();
	void SetThreadName(std::string ThreadName);
	void SetThreadName(const char * ThreadName);

private:
	static unsigned int WINAPI StaticThreadFunc(void * arg);

private:
	HANDLE m_handle;
	Runnable * const m_pRunnable;
	unsigned int m_ThreadID;
	std::string m_ThreadName;
	volatile bool m_bRun;
};

}

#endif