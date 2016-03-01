#include "BaseLine.h"
#include "PxcUtil/CSVTableOperator.h"
#include "PxcUtil/LogCenter.h"
#include "PublicDefinitions/CoreConstantsDef.h"
#include "PublicDefinitions/SpecialFileDef.h"

#define RELEASE_DELAY_TIME CoreConstantsDef::Assets_Consts::c_fAssets_ReleaseDelayTime

CBaseLine::ProductTmpl::ProductTmpl()
{
	pProduct = NULL;
	pLoadingProduct = NULL;
	iNum = 0;
	bToRelease = false;
	fDelayTime = 0.0f;
}

CBaseLine::ProductTmpl::~ProductTmpl()
{
	//
}

CBaseLine::ProcessProduct::ProcessProduct()
{
	pTmpl = NULL;
	pProduct = NULL;
}

CBaseLine::CBaseLine()
{
	m_fCurTickTime = 0.0f;
}

CBaseLine::~CBaseLine()
{
	//
}

bool CBaseLine::Init(const char* szFileName)
{
	PxcUtil::CCSVTableOperator tabop;
	if (!tabop.Load(szFileName))
		return false;

	m_mapTemplates.clear();
	while (tabop.ReadRow())
	{
		CBaseProduct* pProduct = CreateProduct();
		if (!pProduct->Read(tabop))
		{
			delete pProduct;
			Release();
			return false;
		}
		ProductTmpl tmpl;
		tmpl.pProduct = pProduct;
		m_mapTemplates.insert(std::make_pair(pProduct->GetID(), tmpl));
	}
	tabop.Reset();

	m_fCurTickTime = 0.0f;
	return true;
}

void CBaseLine::Release()
{
	std::map<int, ProductTmpl>::iterator iter = m_mapTemplates.begin();
	for (; iter != m_mapTemplates.end(); iter++)
	{
		Update(0.0f);

		if (iter->second.pProduct->IsComplete())
			iter->second.pProduct->UnLoad();
		delete iter->second.pProduct;
	}
	m_mapTemplates.clear();
}

CBaseProduct* CBaseLine::FetchProduct(int iID, CBaseProduct::ELoadType eLoadType, bool bAsyn)
{
	CBaseProduct* pProduct = NULL;
	std::map<int, ProductTmpl>::iterator iter = m_mapTemplates.find(iID);
	if (iter != m_mapTemplates.end())
	{
		ProductTmpl& tmpl = iter->second;
		tmpl.bToRelease = false;

		if (!bAsyn && !tmpl.pProduct->IsComplete())
		{
			if (tmpl.pProduct->Load())
				tmpl.pProduct->OnLoadComplete();
			else
				return NULL;
		}

		if (eLoadType == CBaseProduct::ESharedProduct)
			pProduct = tmpl.pProduct;
		else
		{
			pProduct = tmpl.pProduct->Clone();
			if (pProduct == NULL)
				return NULL;
		}

		pProduct->SetLoadType(eLoadType);
		if (tmpl.pProduct->IsComplete() || !bAsyn)
		{
			pProduct->SetAsynLoad(false);
			if (pProduct->Load())
				pProduct->OnLoadComplete();
			else
			{
				delete pProduct;
				return NULL;
			}
		}
		else
		{
			pProduct->SetAsynLoad(true);
			CRI_SEC(m_lock)
			ProcessProduct loadProduct;
			loadProduct.pTmpl = &tmpl;
			loadProduct.pProduct = pProduct;
			m_lisAsynLoadQueue.push_back(loadProduct);
		}
		tmpl.iNum++;
	}
	return pProduct;
}

void CBaseLine::Discard(CBaseProduct* pProduct)
{
	if (!pProduct)
		return;
	else
	{
		CRI_SEC(m_lock)
		pProduct->SetToDelete(true);
	}

	std::map<int, ProductTmpl>::iterator iter = m_mapTemplates.find(pProduct->GetID());
	if (iter != m_mapTemplates.end())
	{
		ProductTmpl& tmpl = iter->second;
		PXCU_LOG_ASSERT(SpecialFileDef::ELogFile_AssetsTables, (tmpl.iNum > 0))
			<< (tmpl.iNum > 0 ? "" : "资源未被使用");
		if (pProduct->GetLoadType() != CBaseProduct::ESharedProduct)
		{
			bool bReleaseNow = false;
			if (pProduct->IsAsynLoad())
			{
				CRI_SEC(m_lock)
				std::list<ProcessProduct>::iterator itQueue = m_lisAsynLoadQueue.begin();
				for (; itQueue != m_lisAsynLoadQueue.end(); itQueue++)
				{
					if (itQueue->pProduct == pProduct)
					{
						m_lisAsynLoadQueue.erase(itQueue);
						bReleaseNow = true;
						break;
					}
				}
				if (!bReleaseNow && tmpl.pLoadingProduct != pProduct)
				{
					bReleaseNow = true;
					itQueue = m_lisAsynCompleteQueue.begin();
					for (; itQueue != m_lisAsynCompleteQueue.end(); itQueue++)
					{
						if (itQueue->pProduct == pProduct)
						{
							bReleaseNow = false;
							break;
						}
					}
				}
			}
			else
				bReleaseNow = true;

			if (bReleaseNow)
			{
				if (pProduct->IsComplete())
				{
					pProduct->OnBeforeUnLoad();
					pProduct->UnLoad();
				}
				delete pProduct;
				tmpl.iNum--;
			}
		}
		else
		{
			if (pProduct->IsComplete())
				tmpl.iNum--;
		}

		if (tmpl.iNum <= 0)
		{
			tmpl.bToRelease = true;
			tmpl.fDelayTime = RELEASE_DELAY_TIME;
		}
	}
}

void CBaseLine::Update(float dt)
{
	while (true)
	{
		ProcessProduct compProduct;
		bool bDelete = false;

		{
			CRI_SEC(m_lock)
			if (!m_lisAsynCompleteQueue.empty())
			{
				compProduct = m_lisAsynCompleteQueue.front();
				m_lisAsynCompleteQueue.pop_front();
				bDelete = compProduct.pProduct->IsToDelete();
			}
		}

		if (compProduct.pProduct)
		{
			if (compProduct.pProduct->IsLoaded())
				compProduct.pProduct->OnLoadComplete();
			if (compProduct.pTmpl->pProduct->IsLoaded() && compProduct.pTmpl->pProduct->IsComplete())
				compProduct.pTmpl->pProduct->OnLoadComplete();

			if (bDelete)
			{
				compProduct.pProduct->OnBeforeUnLoad();
				compProduct.pProduct->UnLoad();
				delete compProduct.pProduct;
				compProduct.pTmpl->iNum--;
				if (compProduct.pTmpl->iNum <= 0)
				{
					compProduct.pTmpl->bToRelease = true;
					compProduct.pTmpl->fDelayTime = RELEASE_DELAY_TIME;
				}
			}
		}
		else
			break;
	}

	m_fCurTickTime += dt;
	if (m_fCurTickTime >= 1.0f)
	{
		std::map<int, ProductTmpl>::iterator iter = m_mapTemplates.begin();
		for (; iter != m_mapTemplates.end(); iter++)
		{
			ProductTmpl& tmpl = iter->second;
			if (tmpl.bToRelease)
			{
				tmpl.fDelayTime -= m_fCurTickTime;
				if (tmpl.bToRelease && tmpl.fDelayTime <= 0.0f)
				{
					tmpl.pProduct->OnBeforeUnLoad();
					tmpl.pProduct->UnLoad();
					tmpl.bToRelease = false;
				}
			}
		}
		m_fCurTickTime = 0.0f;
	}
}

void CBaseLine::RunLoadProducts()
{
	while (true)
	{
		ProcessProduct loadProduct;

		if (!m_lisAsynLoadQueue.empty())
		{
			CRI_SEC(m_lock)
			loadProduct = m_lisAsynLoadQueue.front();
			m_lisAsynLoadQueue.pop_front();
			if (loadProduct.pTmpl)
				loadProduct.pTmpl->pLoadingProduct = loadProduct.pProduct;
		}
		else
			break;
		if (!loadProduct.pTmpl || !loadProduct.pProduct)
			break;

		if (!loadProduct.pTmpl->pProduct->IsLoaded())
			loadProduct.pTmpl->pProduct->Load();
		if (loadProduct.pTmpl->pProduct->IsLoaded() && !loadProduct.pProduct->IsLoaded())
			loadProduct.pProduct->Load();

		CRI_SEC(m_lock)
		loadProduct.pTmpl->pLoadingProduct = NULL;
		ProcessProduct compProduct;
		compProduct.pTmpl = loadProduct.pTmpl;
		compProduct.pProduct = loadProduct.pProduct;
		m_lisAsynCompleteQueue.push_back(compProduct);
	}
}