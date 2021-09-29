#pragma once
#include "PxcUtil/Singleton.h"
#include "IndustryRow.h"
#include "IndustryTypeRow.h"
#include "OccupationRow.h"
#include "OccupationTypeRow.h"
#include "MajorRow.h"
#include "MajorTypeRow.h"

class CMultiCostTableCenter : public PxcUtil::Singleton<CMultiCostTableCenter>
{
DECLARE_SINGLETON_CONSTRUCTOR(CMultiCostTableCenter)
public:
	~CMultiCostTableCenter();
	bool Init(const std::string& strPath);
	void Release();

	CIndustryRow* GetIndustryRow(int iID);
	CIndustryTypeRow* GetIndustryTypeRow(int iID);
	COccupationRow* GetOccupationRow(int iID);
	COccupationTypeRow* GetOccupationTypeRow(int iID);
	CMajorRow* GetMajorRow(int iID);
	CMajorTypeRow* GetMajorTypeRow(int iID);

	void LoadOccupationToIndustries();
	void GetOccupationRowsByIndustry(int iIndustryID, std::vector<COccupationRow*>& vecOut);
	void GetIndustryRowsByOccupation(int iOccupationID, std::vector<CIndustryRow*>& vecOut);

private:
	std::map<int, CIndustryRow*> m_mapIndustries;
	std::map<int, CIndustryTypeRow*> m_mapIndustryTypes;
	std::map<int, COccupationRow*> m_mapOccupations;
	std::map<int, COccupationTypeRow*> m_mapOccupationTypes;
	std::map<int, CMajorRow*> m_mapMajors;
	std::map<int, CMajorTypeRow*> m_mapMajorTypes;

	std::map<int, std::vector<int>> m_mapTypedIndustries;
	std::map<int, std::vector<int>> m_mapTypedOccupations;
	std::map<int, std::vector<int>> m_mapTypedMajors;
	std::map<int, std::vector<int>> m_mapOccuToIndustry;
};