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
	//FLAGJK

private:
	tMapEvent m_mapEventDelegates;
};