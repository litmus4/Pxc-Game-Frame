#pragma once
#include "PxcUtil/Singleton.h"
#include "PxcUtil/Delegate.h"
#include "EventArgStructs.h"
#include <unordered_map>

class CEventCenter : public PxcUtil::Singleton<CEventCenter>
{
DECLARE_SINGLETON_CONSTRUCTOR(CEventCenter)
public:
	~CEventCenter();
	typedef void (*StaticFunc)(SEventArg*);
	typedef void (IEventDynamicObj::*DynamicFunc)(SEventArg*);
	typedef std::unordered_map<EEventKeyType, PxcUtil::CDelegate<void, SEventArg*>> tMapEvent;

	void Register(EEventKeyType eEventType, StaticFunc pFunc);
	void Register(EEventKeyType eEventType, IEventDynamicObj* pObj, DynamicFunc pFunc);

	template<class T>
	void Register(EEventKeyType eEventType, T* pObj, StaticFunc* pFunc)
	{
		if (!pObj || !pFunc) return;

		tMapEvent::iterator iter = m_mapEventDelegates.find(eEventType);
		if (iter == m_mapEventDelegates.end())
		{
			PxcUtil::CDelegate<void, SEventArg*> Delegate;
			Delegate.SetAutoCycle(true);
			iter = m_mapEventDelegates.insert(std::make_pair(eEventType, Delegate)).first;
		}

		PxcUtil::CDelegate<void, SEventArg*>::FuncObjDyn<T>* pFuncObj =
			new PxcUtil::CDelegate<void, SEventArg*>::FuncObjDyn<T>(pFunc, pObj);
		iter->second.Register(pFuncObj);
	}
	
	void UnRegister(EEventKeyType eEventType, StaticFunc pFunc);
	void UnRegister(EEventKeyType eEventType, IEventDynamicObj* pObj, DynamicFunc pFunc);

	template<class T>
	void UnRegister(EEventKeyType eEventType, T* pObj, StaticFunc pFunc)
	{
		if (!pFunc) return;

		tMapEvent::iterator iter = m_mapEventDelegates.find(eEventType);
		if (iter != m_mapEventDelegates.end())
		{
			PxcUtil::CDelegate<void, SEventArg*>::FuncObjDyn<T> FuncObj(pFunc, pObj);
			iter->second.UnRegister(&FuncObj);
		}
	}

	void SendEvent(EEventKeyType eEventType, SEventArg* pArg);
	void Release();

private:
	tMapEvent m_mapEventDelegates;
};