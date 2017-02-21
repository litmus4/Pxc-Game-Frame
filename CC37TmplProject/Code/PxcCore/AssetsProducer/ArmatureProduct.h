#pragma once
#include "cocos2d.h"
#include "BaseProduct.h"
#include "ArmatureActionInfo.h"
#include "PxcUtil\Delegate.h"

class CSubThreadDataReader;
class CArmatureProduct : public CBaseProduct
{
public:
	enum EPriorityType
	{
		EPriority_None = 0,//0,���°���������
		EPriority_Up,//1,�ϰ�������
		EPriority_Down,//2,�°�������
		EPriority_Whole,//3,���岥��(���ܷ����°���)
		EPriority_Temp,//4,��ʱ����
		EPriority_Num,
	};
	enum EPartType
	{
		EPart_Up = 0,//�ϰ���
		EPart_Down,//�°���
		EPart_Num,
	};

	struct CurrentAction
	{
		enum EState
		{
			ENoAction = -1,//δ��ʼ����
			EEnterAnimation = 0,//���ڲ������붯��
			EAnimation,//���ڲ��ű��ֶ���
			ELeaveAnimation,//���ڲ����г�����
			EEndAction,
		};
		CurrentAction();
		void Reset();

		EState eState;
		CArmatureActionInfo* pActionInfo;
		float fSpeedScale;
		bool bBlend;
	};
};