#pragma once
#include "Scattered.h"

namespace PxcUtil
{

///======================================   
/// 实现单件的模板   
/// 此基类在继承时主要注意要声明此基类的该类实现为其友元类   
/// 历史的尘埃   
/// 2009.3.25   
///======================================   
template<typename T>  
class Singleton  
{  
protected:  
	Singleton()  
	{  

	};  

	static Lock m_Lock;  
	static T* m_T;  

public:  
	static T* GetInstance()  
	{  
		if( m_T == NULL )  
		{  
			AutoLock lock( m_Lock );  
			if( m_T == NULL )  
			{  
				m_T = new T();  
			}  
		}  
		return m_T;  
	}  

	static void DeleteInstance()
	{
		SAFE_DELETE(m_T)
	}

	~Singleton()  
	{  

	}  
};  

template<typename T>  
Lock Singleton<T>::m_Lock = Lock();  

template<typename T>  
T* Singleton<T>::m_T = NULL;

#define DECLARE_SINGLETON_CONSTRUCTOR(className) private:\
	className();\
	friend class PxcUtil::Singleton<className>;

#define DEFINE_SINGLETON_CONSTRUCTOR(className) private:\
	friend class PxcUtil::Singleton<className>;\
	className()

}