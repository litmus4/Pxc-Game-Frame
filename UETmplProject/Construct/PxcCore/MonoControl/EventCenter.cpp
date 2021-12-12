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

void CEventCenter::UnRegister(EEventKeyType eEventType, StaticFunc pFunc)
{
	if (!pFunc) return;

	tMapEvent::iterator iter = m_mapEventDelegates.find(eEventType);
	if (iter != m_mapEventDelegates.end())
	{
		PxcUtil::CDelegate<void, SEventArg*>::FuncObjSta FuncObj(pFunc);
		iter->second.UnRegister(&FuncObj);
	}
}

void CEventCenter::UnRegister(EEventKeyType eEventType, IEventDynamicObj* pObj, DynamicFunc pFunc)
{
	if (!pFunc) return;

	tMapEvent::iterator iter = m_mapEventDelegates.find(eEventType);
	if (iter != m_mapEventDelegates.end())
	{
		PxcUtil::CDelegate<void, SEventArg*>::FuncObjDyn<IEventDynamicObj> FuncObj(pFunc, pObj);
		iter->second.UnRegister(&FuncObj);
	}
}

void CEventCenter::SendEvent(EEventKeyType eEventType, SEventArg* pArg)
{
	tMapEvent::iterator iter = m_mapEventDelegates.find(eEventType);
	if (iter != m_mapEventDelegates.end())
		iter->second(pArg);
}

void CEventCenter::Release()
{
	m_mapEventDelegates.clear();
}