// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/ReplicatedSynchronizer.h"
#include <vector>

CReplicatedSynchronizer::CReplicatedSynchronizer()
{
}

CReplicatedSynchronizer::~CReplicatedSynchronizer()
{
}

void CReplicatedSynchronizer::PushFeatureKeyToServerStack(const std::string& strFeatureKey)
{
	if (IsOnServer())
		m_lisRepFlagServerStack.push_back(strFeatureKey);
}

void CReplicatedSynchronizer::PopFeatureKeyFromServerStack()
{
	if (IsOnServer() && !m_lisRepFlagServerStack.empty())
	{
		std::string& strFeatureKey = m_lisRepFlagServerStack.back();
		m_lisRepFlagServerStack.pop_back();
		m_mapRepSyncFlags.erase(strFeatureKey);
	}
}

bool CReplicatedSynchronizer::TryReplicatedSyncNotifyQueuing(const std::string& strFeatureKey, std::function<void()> fnNotify)
{
	std::map<std::string, uint32>::iterator iter = m_mapRepSyncFlags.find(strFeatureKey);
	if (iter != m_mapRepSyncFlags.end())
	{
		if (iter->second != 0)
		{
			SRepSyncNotify Notify;
			Notify.strFeatureKey = strFeatureKey;
			Notify.fnNotify = fnNotify;
			m_lisRepSyncNotifyQueue.push_back(Notify);
			return true;
		}

		std::list<SRepSyncNotify>::iterator itQueue = m_lisRepSyncNotifyQueue.begin();
		while (itQueue != m_lisRepSyncNotifyQueue.end())
		{
			if (itQueue->strFeatureKey == strFeatureKey)
			{
				itQueue->fnNotify();
				itQueue = m_lisRepSyncNotifyQueue.erase(itQueue);
			}
			else
				itQueue++;
		}
		fnNotify();
		m_mapRepSyncFlags.erase(strFeatureKey);
	}
	return false;
}

void CReplicatedSynchronizer::ResetClientReplicatedFlag(uint32 uFlag)
{
	std::vector<std::string> vecRemovedKeys;
	std::map<std::string, uint32>::iterator iter = m_mapRepSyncFlags.begin();
	for (; iter != m_mapRepSyncFlags.end(); iter++)
	{
		iter->second &= ~uFlag;
		if (iter->second == 0)
		{
			bool bRemove = false;
			std::list<SRepSyncNotify>::iterator itQueue = m_lisRepSyncNotifyQueue.begin();
			while (itQueue != m_lisRepSyncNotifyQueue.end())
			{
				if (itQueue->strFeatureKey == iter->first)
				{
					itQueue->fnNotify();
					itQueue = m_lisRepSyncNotifyQueue.erase(itQueue);
					bRemove = true;
				}
				else
					itQueue++;
			}
			if (bRemove)
				vecRemovedKeys.push_back(iter->first);
		}
	}
	std::vector<std::string>::iterator itRem = vecRemovedKeys.begin();
	for (; itRem != vecRemovedKeys.end(); itRem++)
		m_mapRepSyncFlags.erase(*itRem);
}