#pragma once
#include "PxcUtil/CSVTableOperator.h"
#include "PublicDefinitions\Globalization.h"

class CBaseProduct
{
public:
	enum ELoadType
	{
		ESharedProduct = 0,
		ESharedData,
		EClonedData,
	};

public:
	CBaseProduct();
	virtual ~CBaseProduct();

	int GetID();
	ELoadType GetLoadType();
	void SetLoadType(ELoadType eType);

	virtual bool Read(PxcUtil::CCSVTableOperator& tabop, GlobalDef::ELanguage eLanguage);
	virtual bool Load();
	virtual void UnLoad();
	virtual CBaseProduct* Clone() = 0;

	bool IsAsynLoad();
	void SetAsynLoad(bool b);
	bool IsLoaded();
	bool IsComplete();
	bool IsToDelete();
	void SetToDelete(bool b);

	virtual void OnLoadComplete();
	virtual void OnBeforeUnLoad();

protected:
	int m_iID;
	ELoadType m_eLoadType;

	//1�첽��ǣ�2�Ѽ��ر�ǣ�4����ɱ�ǣ�8ɾ�����
	unsigned int m_uBitFlag;
};