#pragma once
#include "PxcUtil/CSVTableOperator.h"

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

	virtual bool Read(PxcUtil::CCSVTableOperator& tabop);
	virtual bool Load();
	virtual void UnLoad();
	virtual CBaseProduct* Clone() { return NULL; };// = 0;

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

	//1异步标记，2已加载标记，4已完成标记，8删除标记
	unsigned int m_uBitFlag;
};