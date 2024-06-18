// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Systems/RandomGameplaySystem.h"
#include "GameplayRelated/Random/RandomModeComponent.h"

void URandomGameplaySystem::GenerateZGNLayers(TSubclassOf<URandomModeComponent> cComponent)
{
	ClearZGNLayers();

	URandomModeComponent* pDefComp = cComponent.GetDefaultObject();
	if (!IsValid(pDefComp)) return;
	VerifyZGNComponent(pDefComp);

	int32 iMinNodeNum = pDefComp->m_iZGNMinWide * pDefComp->m_iZGNLayerNum;
	TArray<ERandomZGNRoomType> tarrAllTypes = {
		ERandomZGNRoomType::Stage,
		ERandomZGNRoomType::Bonus,
		ERandomZGNRoomType::Support,
		ERandomZGNRoomType::Story
	};
	TArray<ERandomZGNRoomType> tarrTypedTypes;
	int32 iSumMaxNum = 0;
	for (auto& Pair : pDefComp->m_tmapZGNTypedRoomMaxNums)
	{
		iSumMaxNum += Pair.Value;
		m_tmapZGNTypedRoomLeftNums.FindOrAdd(Pair.Key) = Pair.Value;
		tarrAllTypes.Remove(Pair.Key);
		tarrTypedTypes.Add(Pair.Key);
	}
	verify(iMinNodeNum >= iSumMaxNum);

	int32 iLayerIndex = 0;
	do {
		FRandomZGNLayer Layer;

		int32 iIncWide = FMath::RandHelper(pDefComp->m_iZGNMaxWide - pDefComp->m_iZGNMinWide + 1);
		int32 iWide = pDefComp->m_iZGNMinWide + iIncWide;
		iMinNodeNum += iIncWide;

		if (iLayerIndex > 0)
		{
			TArray<TArray<int32>> tarrLinkBacks;
			for (int32 i = 0; i < iWide; ++i)
				tarrLinkBacks.Add(TArray<int32>());
			TArray<int32> tarrLinkLessIndexs;

			FRandomZGNLayer& LastLayer = m_tarrZGNLayers[iLayerIndex - 1];
			for (int32 i = 0; i < LastLayer.tarrNodes.Num(); ++i)
			{
				FRandomZGNNode& Node = LastLayer.tarrNodes[i];

				TArray<int32> tarrAllLinks;
				for (int32 j = 0; j < iWide; ++j)
					tarrAllLinks.Add(j);

				int32 iCurrMaxLink = FMath::RandHelper(pDefComp->m_iZGNNextRoomMaxLink) + 1;
				if (iCurrMaxLink < pDefComp->m_iZGNNextRoomMaxLink) tarrLinkLessIndexs.Add(i);
				do {
					int32 iLinkIndex = FMath::RandHelper(tarrAllLinks.Num());
					Node.tarrNextRoomLinks.Add(tarrAllLinks[iLinkIndex]);
					tarrLinkBacks[tarrAllLinks[iLinkIndex]].Add(i);
					tarrAllLinks.RemoveAt(iLinkIndex);
				} while (tarrAllLinks.Num() > 0 && Node.tarrNextRoomLinks.Num() < iCurrMaxLink);
			}

			//¿ÕÁ¬ÐÞÕý
			for (int32 i = 0; i < tarrLinkBacks.Num(); ++i)
			{
				if (tarrLinkBacks[i].Num() == 0)
				{
					if (tarrLinkLessIndexs.Num() > 0)
					{
						FRandomZGNNode& Node = LastLayer.tarrNodes[tarrLinkLessIndexs[0]];
						Node.tarrNextRoomLinks.Add(i);
						if (Node.tarrNextRoomLinks.Num() >= pDefComp->m_iZGNNextRoomMaxLink)
							tarrLinkLessIndexs.RemoveAt(0);
					}
				}
			}
		}

		for (int32 i = 0; i < iWide; ++i)
		{
			FRandomZGNNode Node;

			float fRange = FMath::RandRange(0.0f, 1.0f);
			if (fRange <= ((float)iSumMaxNum / iMinNodeNum) || tarrAllTypes.Num() == 0)
			{
				do {
					int32 iTypeIndex = FMath::RandHelper(tarrTypedTypes.Num());
					Node.eType = tarrTypedTypes[iTypeIndex];
				} while (m_tmapZGNTypedRoomLeftNums[Node.eType] <= 0);
				m_tmapZGNTypedRoomLeftNums[Node.eType]--;
				iSumMaxNum--;
			}
			else
			{
				int32 iTypeIndex = FMath::RandHelper(tarrAllTypes.Num());
				Node.eType = tarrAllTypes[iTypeIndex];
			}

			Layer.tarrNodes.Add(Node);
			iMinNodeNum--;
		}

		m_tarrZGNLayers.Add(Layer);
		iLayerIndex++;
	} while (m_tarrZGNLayers.Num() < pDefComp->m_iZGNLayerNum);
}

void URandomGameplaySystem::GetZGNLayers(TArray<FRandomZGNLayer>& tarrOutZGNLayers)
{
	tarrOutZGNLayers = m_tarrZGNLayers;
}

void URandomGameplaySystem::ClearZGNLayers()
{
	m_tarrZGNLayers.Empty();
}

void URandomGameplaySystem::Release()
{
	ClearZGNLayers();
}

void URandomGameplaySystem::VerifyZGNComponent(URandomModeComponent* pComp)
{
	verify(pComp->m_iZGNMinWide > 0 && pComp->m_iZGNMaxWide > 0 && pComp->m_iZGNMinWide <= pComp->m_iZGNMaxWide);
	verify(pComp->m_iZGNLayerNum > 0);
	verify(!pComp->m_tmapZGNTypedRoomMaxNums.Contains(ERandomZGNRoomType::Entry));
	for (auto& Pair : pComp->m_tmapZGNStaticTypedRooms)
	{
		if (Pair.Key == ERandomZGNRoomType::Entry)
			verify(Pair.Value.tarrResList.Num() >= 2)
		else
			verify(Pair.Value.tarrResList.Num() > 0);

		for (FSoftObjectPath& ResPath : Pair.Value.tarrResList)
			Pair.Value.tarrLevels.Add(nullptr);
	}
}