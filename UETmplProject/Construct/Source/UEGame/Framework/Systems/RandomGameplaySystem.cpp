// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Systems/RandomGameplaySystem.h"
#include "GameplayRelated/Random/RandomModeComponent.h"

void URandomGameplaySystem::GenerateZGNLayers(TSubclassOf<URandomModeComponent> cComponent)
{
	ClearZGNLayers();
	//TODOJK ConsWait
	int32 iUnlockedTimes = 0;

	URandomModeComponent* pDefComp = cComponent.GetDefaultObject();
	if (!IsValid(pDefComp)) return;
	VerifyZGNComponent(pDefComp);

	int32 iMinNodeNum = pDefComp->m_iZGNMinWide * pDefComp->m_iZGNLayerNum;
	TArray<ERandomZGNRoomType> tarrAllTypes = {
		ERandomZGNRoomType::Stage,
		ERandomZGNRoomType::Bonus,
		ERandomZGNRoomType::Story,
		ERandomZGNRoomType::Support,
		ERandomZGNRoomType::Puzzle
	};
	TArray<ERandomZGNRoomType> tarrTypedTypes;
	TArray<ERandomZGNRoomType> tarrRandomTypes;
	int32 iSumMaxNum = 0;
	for (auto& Pair : pDefComp->m_tmapZGNTypedRoomMaxNums)
	{
		int32 iTypedRoomNum = Pair.Value;
		if (!pDefComp->m_tarrZGNMustMaxRoomTypes.Contains(Pair.Key))
			iTypedRoomNum = FMath::RandHelper(Pair.Value + 1);
		if (Pair.Key == pDefComp->m_eZGNUnlockerRoomType)
		{
			if (iUnlockedTimes > pDefComp->m_tarrZGNUnlockRoomSubTypes.Num())
				iTypedRoomNum = 0;
		}
		else if (Pair.Key == pDefComp->m_eZGNMainUnlockeeRoomType)
		{
			if (iUnlockedTimes == 0)
				iTypedRoomNum = 0;
		}

		if (iTypedRoomNum > 0)
		{
			iSumMaxNum += iTypedRoomNum;
			m_tmapZGNTypedRoomLeftNums.FindOrAdd(Pair.Key) = iTypedRoomNum;
			tarrTypedTypes.Add(Pair.Key);
		}
		else if (!pDefComp->m_tarrZGNMustRandomRoomTypes.Contains(Pair.Key) &&
			Pair.Key != pDefComp->m_eZGNUnlockerRoomType)
			tarrRandomTypes.Add(Pair.Key);
		tarrAllTypes.Remove(Pair.Key);
	}
	if (iUnlockedTimes > pDefComp->m_tarrZGNUnlockRoomSubTypes.Num())
	{
		iSumMaxNum += pDefComp->m_iZGNExtraUnlockeeRoomNum;
		m_tmapZGNTypedRoomLeftNums.FindOrAdd(pDefComp->m_eZGNExtraUnlockeeRoomType) = pDefComp->m_iZGNExtraUnlockeeRoomNum;
		tarrTypedTypes.Add(pDefComp->m_eZGNExtraUnlockeeRoomType);
	}
	for (ERandomZGNRoomType eType : pDefComp->m_tarrZGNMustRandomRoomTypes)
		tarrRandomTypes.Add(eType);
	verify(iMinNodeNum >= iSumMaxNum);

	int32 iLayerIndex = 0;
	do {
		FRandomZGNLayer Layer;

		int32 iIncWide = FMath::RandHelper(pDefComp->m_iZGNMaxWide - pDefComp->m_iZGNMinWide + 1);
		int32 iWide = pDefComp->m_iZGNMinWide + iIncWide;
		iMinNodeNum += iIncWide;

		TArray<TArray<int32>> tarrLinkBacks;
		if (iLayerIndex > 0)
		{
			for (int32 i = 0; i < iWide; ++i)
				tarrLinkBacks.Add(TArray<int32>());
			TArray<int32> tarrLinkLessIndexs;
			TArray<int32> tarrLinkMaxIndexs;

			FRandomZGNLayer& LastLayer = m_tarrZGNLayers[iLayerIndex - 1];
			for (int32 i = 0; i < LastLayer.tarrNodes.Num(); ++i)
			{
				FRandomZGNNode& Node = LastLayer.tarrNodes[i];

				TArray<int32> tarrAllLinks;
				for (int32 j = 0; j < iWide; ++j)
					tarrAllLinks.Add(j);

				int32 iCurrMaxLink = FMath::RandHelper(pDefComp->m_iZGNNextRoomMaxLink) + 1;
				if (iCurrMaxLink < pDefComp->m_iZGNNextRoomMaxLink) tarrLinkLessIndexs.Add(i);
				else tarrLinkMaxIndexs.Add(i);
				do {
					int32 iLinkIndex = FMath::RandHelper(tarrAllLinks.Num());
					Node.tarrNextRoomLinks.Add(tarrAllLinks[iLinkIndex]);
					tarrLinkBacks[tarrAllLinks[iLinkIndex]].Add(i);
					tarrAllLinks.RemoveAt(iLinkIndex);
				} while (tarrAllLinks.Num() > 0 && Node.tarrNextRoomLinks.Num() < iCurrMaxLink);
			}

			//¿ÕÁ¬ÐÞÕý
			bool bHasEmptyLinkBack = false;
			do {
				bHasEmptyLinkBack = false;
				TMap<int32, TArray<int32>> tmapMovedLinkBacks;
				for (int32 i = 0; i < tarrLinkBacks.Num(); ++i)
				{
					if (tarrLinkBacks[i].Num() == 0)
					{
						bHasEmptyLinkBack = true;
						if (tarrLinkLessIndexs.Num() > 0)
						{
							int32 iFirstLessIndex = tarrLinkLessIndexs[0];
							FRandomZGNNode& Node = LastLayer.tarrNodes[iFirstLessIndex];
							Node.tarrNextRoomLinks.Add(i);
							if (Node.tarrNextRoomLinks.Num() >= pDefComp->m_iZGNNextRoomMaxLink)
							{
								tarrLinkLessIndexs.RemoveAt(0);
								tarrLinkMaxIndexs.Add(iFirstLessIndex);
							}
							tarrLinkBacks[i].Add(iFirstLessIndex);
						}
						else
						{
							int32 iFirstMaxIndex = tarrLinkMaxIndexs[0];
							FRandomZGNNode& Node = LastLayer.tarrNodes[iFirstMaxIndex];
							tmapMovedLinkBacks.FindOrAdd(Node.tarrNextRoomLinks[0]).Add(iFirstMaxIndex);
							Node.tarrNextRoomLinks.EmplaceAt(0, i);
							tarrLinkBacks[i].Add(iFirstMaxIndex);
						}
					}
				}
				for (auto& Pair : tmapMovedLinkBacks)
				{
					for (int32 iBack : Pair.Value)
						tarrLinkBacks[Pair.Key].Remove(iBack);
				}
			} while (bHasEmptyLinkBack);
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

					if (Node.eType == pDefComp->m_eZGNMainUnlockeeRoomType)
					{
						int32 iRandMax = FMath::Min(iUnlockedTimes, pDefComp->m_tarrZGNUnlockRoomSubTypes.Num());
						Node.eSubType = pDefComp->m_tarrZGNUnlockRoomSubTypes[FMath::RandHelper(iRandMax)];
					}
					else if (Node.eType == ERandomZGNRoomType::Random)
					{
						Node.tarrRandomTypes = tarrRandomTypes;
						float fRange2 = FMath::RandRange(0.0f, 1.0f);
						Node.fRandomMultiper = (fRange2 <= pDefComp->m_fZGNRandomMultipleProb ? pDefComp->m_fZGNRandomMultiper : 1.0f);
					}
				} while (m_tmapZGNTypedRoomLeftNums[Node.eType] <= 0);
				m_tmapZGNTypedRoomLeftNums[Node.eType]--;
				iSumMaxNum--;
			}
			else
			{
				int32 iTypeIndex = FMath::RandHelper(tarrAllTypes.Num());
				Node.eType = tarrAllTypes[iTypeIndex];
			}

			if (iLayerIndex > 0)
				Node.tarrBackRoomLinks = tarrLinkBacks[i];
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

void URandomGameplaySystem::GetZGNLayersPointer(TArray<FRandomZGNLayer>** pptarrOutZGNLayers)
{
	if (pptarrOutZGNLayers)
		*pptarrOutZGNLayers = &m_tarrZGNLayers;
}

void URandomGameplaySystem::ClearZGNLayers()
{
	m_tarrZGNLayers.Empty();
}

void URandomGameplaySystem::IncreaseUnlockedTimes()
{
	//TODOJK ConsWait
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