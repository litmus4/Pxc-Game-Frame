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
		EPriority_None = 0,//0,上下半身都不优先
		EPriority_Up,//1,上半身优先
		EPriority_Down,//2,下半身优先
		EPriority_Whole,//3,整体播放(不能分上下半身)
		EPriority_Temp,//4,临时播放
		EPriority_Num,
	};
	enum EPartType
	{
		EPart_Up = 0,//上半身
		EPart_Down,//下半身
		EPart_Num,
	};

	struct CurrentAction
	{
		enum EState
		{
			ENoAction = -1,//未开始动作
			EEnterAnimation = 0,//正在播放切入动画
			EAnimation,//正在播放保持动画
			ELeaveAnimation,//正在播放切出动画
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