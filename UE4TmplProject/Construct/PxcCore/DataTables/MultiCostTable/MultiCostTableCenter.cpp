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
	{
		std::map<int, CIndustryRow*>::iterator iter = m_mapIndustries.begin();
		for (; iter != m_mapIndustries.end(); iter++)
		{
			std::map<int, std::vector<int>>::iterator itTyped = m_mapTypedIndustries.find(iter->second->m_iTypeID);
			if (itTyped == m_mapTypedIndustries.end())
			{
				std::vector<int> vecIDs;
				vecIDs.push_back(iter->second->m_iID);
				m_mapTypedIndustries.insert(std::make_pair(iter->second->m_iTypeID, vecIDs));
			}
			else
				itTyped->second.push_back(iter->second->m_iID);
		}
	}
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

	LOADTABLE(Major, strPath, "MultiCostTable", m_mapMajors, m_iID)
	{
		std::map<int, CMajorRow*>::iterator iter = m_mapMajors.begin();
		for (; iter != m_mapMajors.end(); iter++)
		{
			std::map<int, std::vector<int>>::iterator itTyped = m_mapTypedMajors.find(iter->second->m_iTypeID);
			if (itTyped == m_mapTypedMajors.end())
			{
				std::vector<int> vecIDs;
				vecIDs.push_back(iter->second->m_iID);
				m_mapTypedMajors.insert(std::make_pair(iter->second->m_iTypeID, vecIDs));
			}
			else
				itTyped->second.push_back(iter->second->m_iID);
		}
	}
	LOADTABLE(MajorType, strPath, "MultiCostTable", m_mapMajorTypes, m_iID)
}

void CMultiCostTableCenter::Release()
{
	UNLOADTABLE(Industry, m_mapIndustries)
	UNLOADTABLE(IndustryType, m_mapIndustryTypes)
	UNLOADTABLE(Occupation, m_mapOccupations)
	UNLOADTABLE(OccupationType, m_mapOccupationTypes)
	UNLOADTABLE(Major, m_mapMajors)
	UNLOADTABLE(MajorType, m_mapMajorTypes)

	m_mapTypedIndustries.clear();
	m_mapTypedOccupations.clear();
	m_mapTypedMajors.clear();
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

CMajorRow* CMultiCostTableCenter::GetMajorRow(int iID)
{
	std::map<int, CMajorRow*>::iterator iter = m_mapMajors.find(iID);
	if (iter != m_mapMajors.end())
		return iter->second;
	return NULL;
}

CMajorTypeRow* CMultiCostTableCenter::GetMajorTypeRow(int iID)
{
	std::map<int, CMajorTypeRow*>::iterator iter = m_mapMajorTypes.find(iID);
	if (iter != m_mapMajorTypes.end())
		return iter->second;
	return NULL;
}

void CMultiCostTableCenter::LoadOccupationToIndustries()
{
	std::map<int, CIndustryRow*>::iterator iter = m_mapIndustries.begin();
	for (; iter != m_mapIndustries.end(); iter++)
	{
		std::vector<COccupationRow*> vecOccuRows;
		GetOccupationRowsByIndustry(iter->first, vecOccuRows);

		std::vector<COccupationRow*>::iterator itOccr = vecOccuRows.begin();
		for (; itOccr != vecOccuRows.end(); itOccr++)
		{
			std::map<int, std::vector<int>>::iterator itTo = m_mapOccuToIndustry.find((*itOccr)->m_iID);
			if (itTo == m_mapOccuToIndustry.end())
			{
				std::vector<int> vecIDs;
				vecIDs.push_back(iter->first);
				m_mapOccuToIndustry.insert(std::make_pair((*itOccr)->m_iID, vecIDs));
			}
			else
				itTo->second.push_back(iter->first);
		}
	}
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
	std::map<int, std::vector<int>>::iterator iter = m_mapOccuToIndustry.find(iOccupationID);
	if (iter == m_mapOccuToIndustry.end())
	{
		LoadOccupationToIndustries();
		iter = m_mapOccuToIndustry.find(iOccupationID);
		if (iter == m_mapOccuToIndustry.end())
			return;
	}

	std::vector<int>::iterator itSub = iter->second.begin();
	for (; itSub != iter->second.end(); itSub++)
		vecOut.push_back(GetIndustryRow(*itSub));
}