#include "MultiCostTableCenter.h"
#include "..\LoadDefines.h"

CMultiCostTableCenter::CMultiCostTableCenter()
{
	//
}

CMultiCostTableCenter::~CMultiCostTableCenter()
{
	//
}

bool CMultiCostTableCenter::Init(const std::string& strPath)
{
	LOADTABLE(Industry, strPath, "MultiCostTable", m_mapIndustries, m_iID)
	LOADTABLE(IndustryType, strPath, "MultiCostTable", m_mapIndustryTypes, m_iID)
	LOADTABLE(Occupation, strPath, "MultiCostTable", m_mapOccupations, m_iID)
	{
		std::map<int, COccupationRow*>::iterator iter = m_mapOccupations.begin();
		for (; iter != m_mapOccupations.end(); iter++)
		{
			std::map<int, std::vector<int>>::iterator itTyped = m_mapTypedOccupations.find(iter->second->m_iTypeID);
			if (itTyped == m_mapTypedOccupations.end())
			{
				std::vector<int> vecIDs;
				vecIDs.push_back(iter->second->m_iID);
				m_mapTypedOccupations.insert(std::make_pair(iter->second->m_iTypeID, vecIDs));
			}
			else
				itTyped->second.push_back(iter->second->m_iID);
		}
	}
	LOADTABLE(OccupationType, strPath, "MultiCostTable", m_mapOccupationTypes, m_iID)
}

void CMultiCostTableCenter::Release()
{
	UNLOADTABLE(Industry, m_mapIndustries)
	UNLOADTABLE(IndustryType, m_mapIndustryTypes)
	UNLOADTABLE(Occupation, m_mapOccupations)
	UNLOADTABLE(OccupationType, m_mapOccupationTypes)
	m_mapTypedOccupations.clear();
	m_mapOccuToIndustry.clear();
}

CIndustryRow* CMultiCostTableCenter::GetIndustryRow(int iID)
{
	std::map<int, CIndustryRow*>::iterator iter = m_mapIndustries.find(iID);
	if (iter != m_mapIndustries.end())
		return iter->second;
	return NULL;
}

CIndustryTypeRow* CMultiCostTableCenter::GetIndustryTypeRow(int iID)
{
	std::map<int, CIndustryTypeRow*>::iterator iter = m_mapIndustryTypes.find(iID);
	if (iter != m_mapIndustryTypes.end())
		return iter->second;
	return NULL;
}

COccupationRow* CMultiCostTableCenter::GetOccupationRow(int iID)
{
	std::map<int, COccupationRow*>::iterator iter = m_mapOccupations.find(iID);
	if (iter != m_mapOccupations.end())
		return iter->second;
	return NULL;
}

COccupationTypeRow* CMultiCostTableCenter::GetOccupationTypeRow(int iID)
{
	std::map<int, COccupationTypeRow*>::iterator iter = m_mapOccupationTypes.find(iID);
	if (iter != m_mapOccupationTypes.end())
		return iter->second;
	return NULL;
}

void CMultiCostTableCenter::GetOccupationRowsByIndustry(int iIndustryID, std::vector<COccupationRow*>& vecOut)
{
	std::map<int, CIndustryRow*>::iterator iter = m_mapIndustries.find(iIndustryID);
	if (iter == m_mapIndustries.end())
		return;
	
	std::vector<int>::iterator itType = iter->second->m_vecOccupationTypes.begin();
	for (; itType != iter->second->m_vecOccupationTypes.end(); itType++)
	{
		std::map<int, std::vector<int>>::iterator itTyped = m_mapTypedOccupations.find(*itType);
		if (itTyped != m_mapTypedOccupations.end())
		{
			std::vector<int>::iterator itSub = itTyped->second.begin();
			for (; itSub != itTyped->second.end(); itSub++)
			{
				if (std::find(iter->second->m_vecOccupations.begin(), iter->second->m_vecOccupations.end(),
					*itSub) == iter->second->m_vecOccupations.end())
					vecOut.push_back(GetOccupationRow(*itSub));

			}
		}

		std::vector<int>::iterator itOcc = iter->second->m_vecOccupations.begin();
		for (; itOcc != iter->second->m_vecOccupations.end(); itOcc++)
			vecOut.push_back(GetOccupationRow(*itOcc));
	}
}

void CMultiCostTableCenter::GetIndustryRowsByOccupation(int iOccupationID, std::vector<CIndustryRow*>& vecOut)
{
	//FLAGJK
}