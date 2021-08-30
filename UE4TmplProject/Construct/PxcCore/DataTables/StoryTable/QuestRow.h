#pragma once
#include "PxcUtil/CSVTableOperator.h"

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
	bool m_bMainHeadMark;
	int m_iTributaryID;//��֤��浵��Ӧ��
	bool m_bPrevOR;
	bool m_bNextSingle;

	//����������
	//�Ƿ��Զ�����
	//���ܷ־���
	//�ύ������
	//�Ƿ��Զ��ύ
	//�ύ�־���
	//������
};