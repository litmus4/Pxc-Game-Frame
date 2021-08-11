#include "EventCenter.h"

CEventCenter::CEventCenter()
{
	//
}

CEventCenter::~CEventCenter()
{
	//
}

void CEventCenter::Register(EEventKeyType eEventType, StaticFunc pFunc)
{
	if (!pFunc) return;

	tMapEvent::iterator iter = m_mapEventDelegates.find(eEventType);
	if (iter == m_mapEventDelegates.end())
	{
		PxcUtil::CDelegate<void, SEventArg*> Delegate;
		Delegate.SetAutoCycle(true);
		iter = m_mapEventDelegates.insert(std::make_pair(eEventType, Delegate)).first;
	}

	PxcUtil::CDelegate<void, SEventArg*>::FuncObjSta* pFuncObj =
		new PxcUtil::CDelegate<void, SEventArg*>::FuncObjSta(pFunc);
	iter->second.Register(pFuncObj);
}

void CEventCenter::Register(EEventKeyType eEventType, IEventDynamicObj* pObj, DynamicFunc pFunc)
{
	if (!pObj || !pFunc) return;

	tMapEvent::iterator iter = m_mapEventDelegates.find(eEventType);
	if (iter == m_mapEventDelegates.end())
	{
		PxcUtil::CDelegate<void, SEventArg*> Delegate;
		Delegate.SetAutoCycle(true);
		iter = m_mapEventDelegates.insert(std::make_pair(eEventType, Delegate)).first;
	}

	PxcUtil::CDelegate<void, SEventArg*>::FuncObjDyn<IEventDynamicObj>* pFuncObj =
		new PxcUtil::CDelegate<void, SEventArg*>::FuncObjDyn<IEventDynamicObj>(pFunc, pObj);
	iter->second.Register(pFuncObj);
}