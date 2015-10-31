#include "BaseProduct.h"

CBaseProduct::CBaseProduct()
{
	m_iID = -1;
	m_eLoadType = ESharedProduct;
	m_uBitFlag = 0;
}

CBaseProduct::~CBaseProduct()
{
	//
}

int CBaseProduct::GetID()
{
	return m_iID;
}

CBaseProduct::ELoadType CBaseProduct::GetLoadType()
{
	return m_eLoadType;
}

void CBaseProduct::SetLoadType(ELoadType eType)
{
	m_eLoadType = eType;
}

bool CBaseProduct::Read(PxcUtil::CCSVTableOperator& tabop)
{
	return true;
}

bool CBaseProduct::Load()
{
	m_uBitFlag |= 2;
	return true;
}

void CBaseProduct::UnLoad()
{
	m_uBitFlag &= ~4;
	m_uBitFlag &= ~2;
}

bool CBaseProduct::IsAsynLoad()
{
	return (m_uBitFlag & 1);
}

void CBaseProduct::SetAsynLoad(bool b)
{
	m_uBitFlag = b ? (m_uBitFlag | 1) : (m_uBitFlag & ~1);
}

bool CBaseProduct::IsLoaded()
{
	return (m_uBitFlag & 2);
}

bool CBaseProduct::IsComplete()
{
	return (m_uBitFlag & 4);
}

bool CBaseProduct::IsToDelete()
{
	return (m_uBitFlag & 8);
}

void CBaseProduct::SetToDelete(bool b)
{
	m_uBitFlag = b ? (m_uBitFlag | 8) : (m_uBitFlag & ~8);
}

void CBaseProduct::OnLoadComplete()
{
	m_uBitFlag |= 4;
}

void CBaseProduct::OnBeforeUnLoad()
{
	m_uBitFlag &= ~4;
}