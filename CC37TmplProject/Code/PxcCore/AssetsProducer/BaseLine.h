#pragma once
#include "PxcUtil/Scattered.h"
#include "BaseProduct.h"
#include <map>
#include <list>

class CBaseLine
{
public:
	struct ProductTmpl
	{
		ProductTmpl();
		~ProductTmpl();

		CBaseProduct* pProduct;
		CBaseProduct* pLoadingProduct;
		int iNum;
		bool bToRelease;
		float fDelayTime;
	};

	struct ProcessProduct
	{
		ProcessProduct();

		ProductTmpl* pTmpl;
		CBaseProduct* pProduct;
	};

public:
	CBaseLine();
	virtual ~CBaseLine();

	typedef std::map<int, ProductTmpl>::iterator TmplIter;
	TmplIter Begin() { return m_mapTemplates.begin(); }
	TmplIter End() { return m_mapTemplates.end(); }

	virtual bool Init(const char* szFileName);
	virtual void Release();

	virtual void Discard(CBaseProduct* pProduct);
	virtual void Update(float dt);
	virtual void RunLoadProducts();

protected:
	virtual CBaseProduct* FetchProduct(int iID, CBaseProduct::ELoadType eLoadType, bool bAsyn = false);
	virtual CBaseProduct* CreateProduct() { return NULL; };// = 0;//TODO 继承后改为纯虚函数

protected:
	std::map<int, ProductTmpl> m_mapTemplates;
	std::list<ProcessProduct> m_lisAsynLoadQueue;
	std::list<ProcessProduct> m_lisAsynCompleteQueue;
	float m_fCurTickTime;
	PxcUtil::Lock m_lock;
};