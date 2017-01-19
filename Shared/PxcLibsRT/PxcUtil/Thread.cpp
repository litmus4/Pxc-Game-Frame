#include "Thread.h"
#include <windows.h>

using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Windows::System::Threading::Core;

namespace PxcUtil
{

CThread::CThread()
: m_pWorkItem(nullptr)
, m_pPreWorkItem(nullptr)
, m_pRunnable(NULL)
, m_bRun(false)
{
}

CThread::~CThread()
{
}

CThread::CThread(Runnable* pRunnable)
: m_pWorkItem(nullptr)
, m_pPreWorkItem(nullptr)
, m_ThreadName("")
, m_pRunnable(pRunnable)
, m_bRun(false)
{
}

CThread::CThread(const char* ThreadName, Runnable* pRunnable)
: m_pWorkItem(nullptr)
, m_pPreWorkItem(nullptr)
, m_ThreadName(ThreadName)
, m_pRunnable(pRunnable)
, m_bRun(false)
{
}

CThread::CThread(std::string ThreadName, Runnable * pRunnable)
: m_pWorkItem(nullptr)
, m_pPreWorkItem(nullptr)
, m_ThreadName(ThreadName)
, m_pRunnable(pRunnable)
, m_bRun(false)
{
}

bool CThread::Start(bool bSuspend)
{
	if (m_bRun)
		return true;

	WorkItemHandler^ pHandler = ref new WorkItemHandler(
	[this](IAsyncAction^ pWorkItem)
	{
		this->Run();
	}
	);

	if (bSuspend)
	{
		m_pPreWorkItem = ref new PreallocatedWorkItem(pHandler);
	}
	else
	{
		m_pWorkItem = ThreadPool::RunAsync(pHandler);
	}
	m_bRun = (nullptr != m_pWorkItem || nullptr != m_pPreWorkItem);
	return m_bRun;
}

void CThread::Run()
{
	if (!m_bRun)
		return;

	if (NULL != m_pRunnable)
	{
		m_pRunnable->Run();
	}
	m_bRun = false;
}

void CThread::Join(int timeout)
{
	if ((nullptr == m_pWorkItem && nullptr == m_pPreWorkItem)
		|| !m_bRun)
		return;

	if (timeout <= 0)
		timeout = INFINITE;
	//TODOJK
}

void CThread::Resume()
{
	if ((nullptr == m_pWorkItem && nullptr == m_pPreWorkItem)
		|| !m_bRun)
		return;

	if (nullptr != m_pPreWorkItem && nullptr == m_pWorkItem)
	{
		m_pWorkItem = m_pPreWorkItem->RunAsync();
	}
	else
	{
		//TODOJK
	}
}

void CThread::Suspend()
{
	if (nullptr == m_pWorkItem || !m_bRun)
		return;

	//TODOJK
}

bool CThread::Terminate(unsigned long ExitCode)
{
	if ((nullptr == m_pWorkItem && nullptr == m_pPreWorkItem)
		|| !m_bRun)
		return true;

	if (nullptr != m_pPreWorkItem && nullptr == m_pWorkItem)
		m_pWorkItem = m_pPreWorkItem->RunAsync();
	if (nullptr != m_pWorkItem)
	{
		//TODOJK Cancel还是Close？前者需要线程内判断m_pWorkItem->Status，后者不清楚是否强制终止
		m_pWorkItem->Cancel();
		return true;
	}
	return false;
}

unsigned int CThread::GetThreadID()
{
	//TODOJK 回头实现用IDPool自动生成线程ID
	return 0;
}

std::string CThread::GetThreadName()
{
	return m_ThreadName;
}

void CThread::SetThreadName(std::string ThreadName)
{
	m_ThreadName = ThreadName;
}

void CThread::SetThreadName(const char* ThreadName)
{
	if (NULL == ThreadName)
	{
		m_ThreadName = "";
	}
	else
	{
		m_ThreadName = ThreadName;
	}
}

bool CThread::IsCanceled()
{
	if (nullptr != m_pWorkItem)
		return (m_pWorkItem->Status == AsyncStatus::Canceled);
	return false;
}

}