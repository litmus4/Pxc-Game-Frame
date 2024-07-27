// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayRelated/Random/RandomModeComponent.h"
#include "Framework/PxcNativeLibrary.h"
#include "Framework/PXCycleInstance.h"
#include "Framework/Systems/RandomGameplaySystem.h"

// Sets default values for this component's properties
URandomModeComponent::URandomModeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void URandomModeComponent::SetRandomModeType(ERandomModeType Type)
{
	m_eModeType = Type;
}

void URandomModeComponent::RunRandomTeleport(class AIndexedToAssignActor* pActor, int32 iSelectedIndex)
{
	check(pActor);

	if (pActor->m_iAssignedNumber >= 0)
	{
		ERandomZGNRoomType eType = (ERandomZGNRoomType)pActor->m_iAssignedNumber;
		bool bRandomMainUnlockee = false;
		ResetZGNRoomState();
		if (eType == ERandomZGNRoomType::Random)
		{
			FRandomZGNNode* pNode = static_cast<FRandomZGNNode*>(pActor->m_pAssignedData);
			m_ZGNRoomState.eRandomType = pNode->tarrRandomTypes[FMath::RandHelper(pNode->tarrRandomTypes.Num())];
			m_ZGNRoomState.fRandomMultiper = pNode->fRandomMultiper;
			if (m_ZGNRoomState.eRandomType == m_eZGNMainUnlockeeRoomType)
			{
				//TODOJK ConsWait
				int32 iUnlockedTimes = 0;
				int32 iRandMax = FMath::Min(iUnlockedTimes, m_tarrZGNUnlockRoomSubTypes.Num());
				m_ZGNRoomState.eSubType = m_tarrZGNUnlockRoomSubTypes[FMath::RandHelper(iRandMax)];
				bRandomMainUnlockee = true;
			}
			eType = m_ZGNRoomState.eRandomType;
		}
		FRandomSubLevelList* pLevelList = m_tmapZGNStaticTypedRooms.Find(eType);

		if (pLevelList)
		{
			FString sLevelName;
			if (eType != ERandomZGNRoomType::Entry)
			{
				if (iSelectedIndex >= 0 && pLevelList->tarrResList.IsValidIndex(iSelectedIndex))
					sLevelName = pLevelList->tarrResList[iSelectedIndex].GetAssetName();
				else
					sLevelName = pLevelList->tarrResList[FMath::RandHelper(pLevelList->tarrResList.Num())].GetAssetName();

				if (eType == m_eZGNMainUnlockeeRoomType && !bRandomMainUnlockee)
					m_ZGNRoomState.eSubType = static_cast<FRandomZGNNode*>(pActor->m_pAssignedData)->eSubType;

				m_iCurrLayerIndex++;
				m_iCurrNodeIndex = pActor->m_iIndexInWorld;
			}
			else
			{
				sLevelName = pLevelList->tarrResList[1].GetAssetName();

				m_iCurrLayerIndex = -1;
				m_iCurrNodeIndex = -1;
			}

			pActor->RunAssignName(FName(sLevelName));
		}
	}
}

void URandomModeComponent::RemoveFromGameMode()
{
	DestroyComponent();
}

// Called when the game starts
void URandomModeComponent::BeginPlay()
{
	Super::BeginPlay();

	ResetZGNRoomState();
}

// Called every frame
void URandomModeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void URandomModeComponent::SetStreamingLevelInstance(ULevelStreaming* pLSObj)
{
	if (m_eModeType == ERandomModeType::Unknown)
		return;

	check(pLSObj);
	int32 iGangIndex = 0;
	pLSObj->PackageNameToLoad.ToString().FindLastChar(TCHAR('/'), iGangIndex);
	FString sLevelName = pLSObj->PackageNameToLoad.ToString().RightChop(iGangIndex + 1);

	if (m_eModeType == ERandomModeType::ZGN)
	{
		for (auto& Pair : m_tmapZGNStaticTypedRooms)
		{
			bool bBreak = false;
			int32 i = 0;
			for (; i < Pair.Value.tarrResList.Num(); ++i)
			{
				FSoftObjectPath& ResPath = Pair.Value.tarrResList[i];
				if (ResPath.GetAssetName() == sLevelName && Pair.Value.tarrLevels[i].IsNull())
				{
					Pair.Value.tarrLevels.EmplaceAt(i, pLSObj->GetWorldAsset());
					bBreak = true;
					break;
				}
			}

			if (bBreak)
			{
				switch (Pair.Key)
				{
				case ERandomZGNRoomType::Entry:
				{
					m_iCurrLayerIndex = -1;
					m_iCurrNodeIndex = -1;
					if (i > 0) break;
				}
				default:
				{
					TArray<AActor*> tarrTeleportActors;
					TArray<AIndexedToAssignActor*> tarrTeleportIActors;
					FPxcNativeLibrary::GetAllActorsInSubLevel(pLSObj->GetWorld(), pLSObj, m_cTeleportActor, tarrTeleportActors,
						[&tarrTeleportIActors](AActor* pActor) {
							AIndexedToAssignActor* pIActor = CastChecked<AIndexedToAssignActor>(pActor);
							if (pIActor->m_iIndexInWorld >= 0)
							{
								tarrTeleportIActors.Add(pIActor);
								return true;
							}
							return false;
						});
					tarrTeleportIActors.Sort([](const AIndexedToAssignActor& pActor1, const AIndexedToAssignActor& pActor2) {
						return pActor1.m_iIndexInWorld < pActor2.m_iIndexInWorld;
					});

					UPXCycleInstance* pGI = GetWorld()->GetGameInstance<UPXCycleInstance>();
					check(pGI);
					TArray<FRandomZGNLayer>* ptarrZGNLayers = nullptr;
					pGI->GetRandomGameplaySystem()->GetZGNLayersPointer(&ptarrZGNLayers);

					FRandomZGNLayer* pLayer = (ptarrZGNLayers->IsValidIndex(m_iCurrLayerIndex + 1) ? &(*ptarrZGNLayers)[m_iCurrLayerIndex + 1] : nullptr);
					for (int32 j = 0; j < tarrTeleportIActors.Num(); ++j)
					{
						AIndexedToAssignActor* pIActor = tarrTeleportIActors[j];
						if (pLayer && m_iCurrLayerIndex >= 0)//中间层
						{
							FRandomZGNLayer& CurrLayer = (*ptarrZGNLayers)[m_iCurrLayerIndex];
							FRandomZGNNode& CurrNode = CurrLayer.tarrNodes[m_iCurrNodeIndex];
							if (CurrNode.tarrNextRoomLinks.Find(j) != INDEX_NONE && pLayer->tarrNodes.IsValidIndex(j))
							{
								pIActor->RunAssignNumber((int32)pLayer->tarrNodes[j].eType);
								if (pLayer->tarrNodes[j].eType == m_eZGNMainUnlockeeRoomType ||
									pLayer->tarrNodes[j].eType == ERandomZGNRoomType::Random)
									pIActor->RunAssignData((void*)&pLayer->tarrNodes[j]);
							}
						}
						else if (pLayer)//入口
						{
							if (pLayer->tarrNodes.IsValidIndex(j))
							{
								pIActor->RunAssignNumber((int32)pLayer->tarrNodes[j].eType);
								if (pLayer->tarrNodes[j].eType == m_eZGNMainUnlockeeRoomType ||
									pLayer->tarrNodes[j].eType == ERandomZGNRoomType::Random)
									pIActor->RunAssignData((void*)&pLayer->tarrNodes[j]);
							}
						}
						else//最后一层
							pIActor->RunAssignNumber(0);
					}
				}
				}
				break;
			}
		}
	}
}

void URandomModeComponent::ResetZGNRoomState()
{
	m_ZGNRoomState.eSubType = ERandomZGNRoomSubType::Support_0;
	m_ZGNRoomState.eRandomType = ERandomZGNRoomType::Entry;
	m_ZGNRoomState.fRandomMultiper = 0.0f;
}