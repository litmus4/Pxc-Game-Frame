#pragma once
#include <stdio.h>
#include <windows.h>

#define SAFE_DELETE(p) if (p){ delete p; p = NULL; }
#define SAFE_DELETE_ARRAY(p) if (p){ delete[] p; p = NULL; }

#define FLOAT_PRECISION 0.000001f

#define PXCU_PI 3.14159265f

namespace PxcUtil
{

class Lock
{
public:
    Lock()
    {
        InitializeCriticalSectionEx( &m_Critic, 0, 0 );  
    }
    ~Lock()
    {
        DeleteCriticalSection( &m_Critic );
    }
    //////////////////////////////////////////////////////////////////////////  
    /*
     *  进入临界区
     */
    void InLock()
    {              
            EnterCriticalSection( &m_Critic );       
    }
    //////////////////////////////////////////////////////////////////////////  
    /*
     *  离开临界区
     */
    void OutLock()
    {
               
            LeaveCriticalSection( &m_Critic );        
    }
private:
    CRITICAL_SECTION m_Critic; 
};

class AutoLock
{
public:
    AutoLock( Lock &pLock ):m_pLock( pLock )
    {
        pLock.InLock();
    }
    ~AutoLock()
    {
        m_pLock.OutLock();
    }
private:
    Lock &m_pLock;
};

#define CRI_SEC(lock) PxcUtil::AutoLock _csalock_(lock);

}