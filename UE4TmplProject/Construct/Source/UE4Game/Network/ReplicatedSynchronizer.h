// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <string>
#include <functional>
#include <map>
#include <list>

/**
 * 
 */
class UE4GAME_API CReplicatedSynchronizer
{
public:
	CReplicatedSynchronizer();
	virtual ~CReplicatedSynchronizer();

	template<class T>
	void RegisterReplicatedSync()
	{
		if (CanRegisterReplicatedSync() && !m_lisRepFlagServerStack.empty())
		{
			std::string& strKey = m_lisRepFlagServerStack.back();
			if (m_mapRepSyncFlags.find(strKey) != m_mapRepSyncFlags.end())
				Cast<T>(this)->ClientRegisterReplicatedSync(strKey, m_mapRepSyncFlags[strKey]);
		}
	}

	virtual bool CanRegisterReplicatedSync() { return false; }
	virtual bool IsOnServer() { return false; }

protected:
	struct SRepSyncNotify
	{
		std::string strFeatureKey;
		std::function<void()> fnNotify;
	};

	void PushFeatureKeyToServerStack(const std::string& strFeatureKey);
	void PopFeatureKeyFromServerStack();

	FORCEINLINE void ReadyServerReplicatedFlag(uint32 uFlag)
	{
		if (!m_lisRepFlagServerStack.empty())
		{
			std::string& strKey = m_lisRepFlagServerStack.back();
			std::map<std::string, uint32>::iterator iter = m_mapRepSyncFlags.find(strKey);
			if (iter != m_mapRepSyncFlags.end())
				iter->second |= uFlag;
			else
				m_mapRepSyncFlags.insert(std::make_pair(strKey, 0));
		}
	}

	bool TryReplicatedSyncNotifyQueuing(const std::string& strFeatureKey, std::function<void()> fnNotify);
	void ResetClientReplicatedFlag(uint32 uFlag);

protected:
	std::map<std::string, uint32> m_mapRepSyncFlags;
	std::list<std::string> m_lisRepFlagServerStack;
	std::list<SRepSyncNotify> m_lisRepSyncNotifyQueue;
};
