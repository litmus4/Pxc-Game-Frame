#pragma once
#include "PxcUtil/CSVTableOperator.h"
#include "PublicDefinitions/StoryDef.h"

class CQuestRow
{
public:
	CQuestRow();
	~CQuestRow();
	void Read(PxcUtil::CCSVTableOperator& tabop);

	int m_iID;
	int m_iNameTextID;
	int m_iDescTextID;
	int m_iContentStoryTextID;

	int m_iGraphID;
	int m_iChapterID;
	int m_iPrevQuestID;
	std::vector<int> m_vecSubPrevQuestIDs;
	int m_iHeadLevel;
	int m_iParentHeadQuestID;
	int m_iTributaryID;//��֤��浵��Ӧ��
	bool m_bPrevOR;
	StoryDef::EPrevConfluentType m_ePrevConfType;
	bool m_bNextSingle;

	//����������
	//�Ƿ��Զ�����
	//���ܷ־���
	//�ύ������
	//�Ƿ��Զ��ύ
	//�ύ�־���
	//������
};