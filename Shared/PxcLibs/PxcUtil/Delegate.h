#pragma once
#include "Scattered.h"
#include <vector>

namespace PxcUtil
{

template<typename TRet, typename TArg>
class CDelegate
{
public:
	class FuncObj
	{
	public:
		virtual ~FuncObj(){}
		virtual TRet operator()(TArg arg) = 0;
		virtual bool operator==(const FuncObj& other){ return false; }
	};

	class FuncObjSta : public FuncObj
	{
	public:
		typedef TRet (*Func)(TArg);
		FuncObjSta(Func pFunc) : m_Func(pFunc){}

		virtual TRet operator()(TArg arg)
		{
			return m_Func(arg);
		}

		virtual bool operator==(const FuncObj& other)
		{
			const FuncObjSta* pOtherSta = dynamic_cast<const FuncObjSta*>(&other);
			if (pOtherSta)
				return (m_Func == pOtherSta->m_Func);
			return false;
		}

	private:
		Func m_Func;
	};

	template<typename T>
	class FuncObjDyn : public FuncObj
	{
	public:
		typedef TRet (T::*Func)(TArg);
		FuncObjDyn(Func pFunc, T* pObj) : m_Func(pFunc), m_pObj(pObj){}

		virtual TRet operator()(TArg arg)
		{
			return (m_pObj->*m_Func)(arg);
		}

		virtual bool operator==(const FuncObj& other)
		{
			const FuncObjDyn* pOtherDyn = dynamic_cast<const FuncObjDyn*>(&other);
			if (pOtherDyn)
				return ((m_Func == pOtherDyn->m_Func) && (m_pObj == pOtherDyn->m_pObj));
			return false;
		}

	private:
		Func m_Func;
		T* m_pObj;
	};

	CDelegate() : m_bAutoCycle(false){}

	~CDelegate()
	{
		Clear();
	}

	void Register(FuncObj* pFuncObj)
	{
		CRI_SEC(m_lock)
		m_vecFuncObjs.push_back(pFuncObj);
	}

	bool UnRegister(const FuncObj* pFuncObj)
	{
		CRI_SEC(m_lock)
		std::vector<FuncObj*>::iterator iter = m_vecFuncObjs.begin();
		for (; iter != m_vecFuncObjs.end(); iter++)
		{
			if (*(*iter) == *pFuncObj)
			{
				m_vecFuncObjs.erase(iter);
				return true;
			}
		}
		return false;
	}

	TRet operator()(TArg arg)
	{
		CRI_SEC(m_lock)
		std::vector<FuncObj*>::iterator iter = m_vecFuncObjs.begin();
		for (; iter != m_vecFuncObjs.end(); iter++)
		{
			if (iter + 1 == m_vecFuncObjs.end())
				return (*(*iter))(arg);
			else
				(*(*iter))(arg);
		}
	}

	void Clear()
	{
		CRI_SEC(m_lock)
		if (m_bAutoCycle)
		{
			std::vector<FuncObj*>::iterator iter = m_vecFuncObjs.begin();
			for (; iter != m_vecFuncObjs.end(); iter++)
				SAFE_DELETE(*iter)
		}
		m_vecFuncObjs.clear();
	}

	void SetAutoCycle(bool b)
	{
		m_bAutoCycle = b;
	}

private:
	std::vector<FuncObj*> m_vecFuncObjs;
	bool m_bAutoCycle;
	Lock m_lock;
};

}