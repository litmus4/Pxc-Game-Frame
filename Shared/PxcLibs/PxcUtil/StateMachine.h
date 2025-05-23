#pragma once
#include "Scattered.h"
#include <map>

namespace PxcUtil
{

class IState
{
public:
	IState() {}
	virtual ~IState() {}
	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;
};

template<typename TState, typename TEvent>
class CStateMachine
{
public:
	CStateMachine() : m_pCurrent(NULL), m_bAutoCycle(false){}
	~CStateMachine()
	{
		Clear();
	}

	void AddState(TState* pState)
	{
		CRI_SEC(m_lock)
		typename std::map<TState*, tEventMap>::iterator iter = m_mapStates.find(pState);
		if (iter == m_mapStates.end())
		{
			m_mapStates.insert(std::make_pair(pState, tEventMap()));
		}
	}

	bool AddTransfer(TState* pSrcState, TEvent event, TState* pDstState)
	{
		CRI_SEC(m_lock)
		typename std::map<TState*, tEventMap>::iterator itSrc = m_mapStates.find(pSrcState);
		if (itSrc == m_mapStates.end())
			return false;

		typename std::map<TState*, tEventMap>::iterator itDst = m_mapStates.find(pDstState);
		if (itDst == m_mapStates.end())
			return false;

		typename tEventMap::iterator iter = itSrc->second.find(event);
		if (iter == itSrc->second.end())
		{
			itSrc->second.insert(std::make_pair(event, pDstState));
			return true;
		}
		return false;
	}

	bool SetState(TState* pState, bool bForce = false)
	{
		CRI_SEC(m_lock)
		if (pState == m_pCurrent)
			return true;

		if (!bForce)
		{
			typename std::map<TState*, tEventMap>::iterator itCur = m_mapStates.find(m_pCurrent);
			if (itCur == m_mapStates.end())
				return false;

			typename tEventMap::iterator itNext = itCur->second.begin();
			for (; itNext != itCur->second.end(); itNext++)
			{
				if (itNext->second == pState)
					break;
			}
			if (itNext == itCur->second.end())
				return false;
		}

		typename std::map<TState*, tEventMap>::iterator iter = m_mapStates.find(pState);
		if (iter != m_mapStates.end())
		{
			if constexpr (std::is_same_v<TState, IState>)
				if (m_pCurrent) m_pCurrent->OnExit();
			m_pCurrent = pState;
			if constexpr (std::is_same_v<TState, IState>)
				pState->OnEnter();
			return true;
		}
		return false;
	}

	bool TriggerEvent(TEvent event)
	{
		CRI_SEC(m_lock)
		typename std::map<TState*, tEventMap>::iterator itCur = m_mapStates.find(m_pCurrent);
		if (itCur == m_mapStates.end())
			return false;

		typename tEventMap::iterator itNext = itCur->second.find(event);
		if (itNext != itCur->second.end())
		{
			return SetState(itNext->second, true);
		}
		return false;
	}

	int GetStateNum()
	{
		return m_mapStates.size();
	}

	TState* GetCurrentState()
	{
		return m_pCurrent;
	}

	void Clear()
	{
		CRI_SEC(m_lock)
		if (m_bAutoCycle)
		{
			typename std::map<TState*, tEventMap>::iterator iter = m_mapStates.begin();
			for (; iter != m_mapStates.end(); iter++)
			{
				if (iter->first)
					delete iter->first;
			}
		}
		m_mapStates.clear();
	}

	void SetAutoCycle(bool b)
	{
		m_bAutoCycle = b;
	}

private:
	typedef std::map<TEvent, TState*> tEventMap;

	std::map<TState*, tEventMap> m_mapStates;
	TState* m_pCurrent;
	bool m_bAutoCycle;
	Lock m_lock;
};

}