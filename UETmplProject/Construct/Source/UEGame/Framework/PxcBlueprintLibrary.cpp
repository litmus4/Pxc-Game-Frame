// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/PxcBlueprintLibrary.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/KismetInputLibrary.h"
#include "Styling/SlateBrush.h"
#include "GameplayAbility/PxcAbilitySystemComponent.h"
#include "GameplayAbility/ExecutionCalculations/ExtentionExecution.h"
#include "PXCycleInstance.h"
#include "PxcGameMode.h"
#include "PxcGameConfig.h"
#include "LevelMgrs/VirtualGroupMgr.h"
#include "PublicDefinitions/AssetsDef.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "DataTables/OtherTable/OtherTableCenter.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

DEFINE_LOG_CATEGORY(LogPxcBPLib);

void UPxcBlueprintLibrary::SetVirtGrpRTDFeatureBack(UObject* pWCO, UPARAM(ref) FVirtGrpRTDFeature& Feature)
{
	UWorld* pWorld = (IsValid(pWCO) ? pWCO->GetWorld() : nullptr);
	if (!pWorld || !pWorld->IsGameWorld())
		return;

	UVirtualGroupMgr* pManager = CastChecked<APxcGameMode>(pWorld->GetAuthGameMode())->GetVirtualGroupMgr();
	check(pManager);
	FVirtGrpRTDFeature* pFeature = pManager->GetFeatureFromGroup<FVirtGrpRTDFeature>(Feature.GetUsage(), Feature.GroupName);
	if (pFeature)
		pFeature->fTimeDilation = Feature.fTimeDilation;
}

int64 UPxcBlueprintLibrary::MakePassiveAttackExListenerSignature(float fAttackAdd, float fAttackMulti, FSharedSignature& OutSig)
{
	TSharedPtr<FPassiveAttackExListener> pListener = MakeShared<FPassiveAttackExListener>();
	pListener->fAttackAdd = fAttackAdd;
	pListener->fAttackMulti = fAttackMulti;
	OutSig = FSharedSignature(*pListener);
	OutSig->GenerateUidLocal();
	return OutSig->lUid;
}

FActiveGameplayEffectHandle UPxcBlueprintLibrary::ApplyExtentionGESpecWithReturn(UAbilitySystemComponent* pASC, const FGameplayEffectSpecHandle& SpecHandle,
	FActiveGameplayEffectHandle StackedHandle, TMap<FString, float>& tmapRet)
{
	check(pASC);
	auto Handle = pASC->BP_ApplyGameplayEffectSpecToSelf(SpecHandle);
	UPxcAbilitySystemComponent* pPASC = Cast<UPxcAbilitySystemComponent>(pASC);
	if (pPASC)
	{
		if (Handle.WasSuccessfullyApplied())
		{
			pPASC->ForEachGEExtentionReturnToStack(StackedHandle.IsValid() ? StackedHandle : Handle,
				[&SpecHandle, &tmapRet](const std::string& strName, float fValue) {
					SpecHandle.Data->SetSetByCallerMagnitude(FName(strName.c_str()), fValue);
					tmapRet.Add(FString(strName.c_str()), fValue);
				}
			);
		}
		else
		{
			std::map<std::string, float>::iterator iter = pPASC->m_mapGEExtentionReturn.begin();
			for (; iter != pPASC->m_mapGEExtentionReturn.end(); iter++)
			{
				SpecHandle.Data->SetSetByCallerMagnitude(FName(iter->first.c_str()), iter->second);
				tmapRet.Add(FString(iter->first.c_str()), iter->second);
			}
			pPASC->m_mapGEExtentionReturn.clear();
		}
	}
	return Handle;
}

void UPxcBlueprintLibrary::DiscardExtentionGEByStackedHandle(UAbilitySystemComponent* pASC, FActiveGameplayEffectHandle StackedHandle)
{
	check(pASC);
	const FActiveGameplayEffect* pActiveEffect = pASC->GetActiveGameplayEffect(StackedHandle);
	if (pActiveEffect)
		UExtentionExecution::Discard(pASC, pActiveEffect->Spec, pActiveEffect->Handle);
}

void UPxcBlueprintLibrary::CutGEExtentionVariable(float fVariable, TArray<int32> tarrDecSplits, TArray<int32>& tarrOutValues)
{
	for (int32& iDecSplit : tarrDecSplits)
	{
		check(iDecSplit > 0);
		float fDiv = FMath::Pow(10, iDecSplit);
		fVariable *= fDiv;
		int32 iCurValue = ((int32)fVariable) % ((int32)fDiv);
		tarrOutValues.Add(iCurValue);
	}
}

int32 UPxcBlueprintLibrary::PushIdentifiedLessWhenGE(UPARAM(ref) FIdentifiedLessStack& Stack, float fValue)
{
	return Stack.PushValue(fValue);
}

void UPxcBlueprintLibrary::PopIdentifiedLessWhenGE(UPARAM(ref) FIdentifiedLessStack& Stack, int32 iUid)
{
	Stack.PopValue(iUid);
}

bool UPxcBlueprintLibrary::GetIdentifiedLessTop(const FIdentifiedLessStack& Stack, float& fOutValue)
{
	std::set<float>::const_iterator iter = Stack.set.begin();
	if (iter != Stack.set.end())
	{
		fOutValue = *iter;
		return true;
	}
	return false;
}

int32 UPxcBlueprintLibrary::PushIdentifiedGreaterWhenGE(UPARAM(ref) FIdentifiedGreaterStack& Stack, float fValue)
{
	return Stack.PushValue(fValue);
}

void UPxcBlueprintLibrary::PopIdentifiedGreaterWhenGE(UPARAM(ref) FIdentifiedGreaterStack& Stack, int32 iUid)
{
	Stack.PopValue(iUid);
}

bool UPxcBlueprintLibrary::GetIdentifiedGreaterTop(const FIdentifiedGreaterStack& Stack, float& fOutValue)
{
	std::set<float>::const_iterator iter = Stack.set.begin();
	if (iter != Stack.set.end())
	{
		fOutValue = *iter;
		return true;
	}
	return false;
}

FName UPxcBlueprintLibrary::Key_GetName(const FKey& Key)
{
	return Key.GetFName();
}

void UPxcBlueprintLibrary::Key_GetAllActionMappingByName(const FName& ActionName,
	TArray<FInputActionKeyMapping>& tarrOutMappings, TArray<FGamepadCombineMapping>& tarrOutGamepadCombMappings)
{
	const UInputSettings* pSetting = GetDefault<UInputSettings>();
	pSetting->GetActionMappingByName(ActionName, tarrOutMappings);

	auto& tmapGamepadCombineMappings = GetDefault<UPxcGameConfig>()->GetGamepadCombineMappings();
	const TArray<FGamepadCombineMapping>* pCombMappingList = tmapGamepadCombineMappings.Find(ActionName);
	if (pCombMappingList)
	{
		for (const FGamepadCombineMapping& CombineMapping : *pCombMappingList)
			tarrOutGamepadCombMappings.Add(CombineMapping);
	}
}

int32 UPxcBlueprintLibrary::Key_GetFirstAdaptiveAcMapping(const TArray<FInputActionKeyMapping>& tarrMappings, const TArray<FGamepadCombineMapping>& tarrGamepadCombMappings,
	FInputActionKeyMapping& OutMapping, FGamepadCombineMapping& OutGamepadCombMapping)
{
	UPXCycleInstance* pGI = Cast<UPXCycleInstance>(GWorld ? GWorld->GetGameInstance() : nullptr);
	if (!pGI) return 0;

	for (const FInputActionKeyMapping& Mapping : tarrMappings)
	{
		bool bCurKb = UKismetInputLibrary::Key_IsKeyboardKey(Mapping.Key);
		auto pComb = tarrGamepadCombMappings.FindByPredicate([&Mapping](const FGamepadCombineMapping& CurCM)->bool {
			return CurCM.MainKey == Mapping.Key;
		});
		if (pGI->m_bKeyboardRuntime ? bCurKb : (!bCurKb && !pComb))
		{
			OutMapping = Mapping;
			return 1;
		}
	}
	if (pGI->m_bKeyboardRuntime) return 0;

	for (const FGamepadCombineMapping& CombineMapping : tarrGamepadCombMappings)
	{
		if (CombineMapping.MainKey.IsGamepadKey())
		{
			OutGamepadCombMapping = CombineMapping;
			return -1;
		}
	}

	return 0;
}

int32 UPxcBlueprintLibrary::Key_GetIndexedDevisedAcMapping(const TArray<FInputActionKeyMapping>& tarrMappings, const TArray<FGamepadCombineMapping>& tarrGamepadCombMappings,
	bool bDevisedKeyboard, int32 iIndex, bool bGamepadCombBoth, FInputActionKeyMapping& OutMapping, FGamepadCombineMapping& OutGamepadCombMapping)
{
	int32 i = 0;

	bool bBothFound = false;
	for (const FInputActionKeyMapping& Mapping : tarrMappings)
	{
		bool bCurKb = UKismetInputLibrary::Key_IsKeyboardKey(Mapping.Key);
		int32 iCombIndex = tarrGamepadCombMappings.IndexOfByPredicate([&Mapping](const FGamepadCombineMapping& CurCM)->bool {
			return CurCM.MainKey == Mapping.Key;
		});
		if (bDevisedKeyboard ? bCurKb : (!bCurKb && (bGamepadCombBoth || iCombIndex == INDEX_NONE)))
		{
			if (i == iIndex)
			{
				OutMapping = Mapping;
				if (iCombIndex != INDEX_NONE)//Both
				{
					iIndex = tarrMappings.Num() + iCombIndex;
					bBothFound = true;
					i++; break;
				}
				else return 1;
			}
			i++;
		}
	}
	if (bDevisedKeyboard) return 0;
	if (bGamepadCombBoth && !bBothFound) return 0;

	for (const FGamepadCombineMapping& CombineMapping : tarrGamepadCombMappings)
	{
		if (CombineMapping.MainKey.IsGamepadKey())
		{
			if (i == iIndex)
			{
				OutGamepadCombMapping = CombineMapping;
				return -1;
			}
			i++;
		}
	}

	return 0;
}

void UPxcBlueprintLibrary::Key_GetAllAxisMappingByName(const FName& AxisName, bool bPositiveDir, TArray<FInputAxisKeyMapping>& tarrOutMappings)
{
	const UInputSettings* pSetting = GetDefault<UInputSettings>();
	TArray<FInputAxisKeyMapping> tarrAllMappings;
	pSetting->GetAxisMappingByName(AxisName, tarrAllMappings);
	for (FInputAxisKeyMapping& Mapping : tarrAllMappings)
	{
		if ((bPositiveDir ? Mapping.Scale >= 0.0f : Mapping.Scale < 0.0f) || Mapping.Key.IsAxis1D()/*摇杆方向一个顶俩*/)
			tarrOutMappings.Add(Mapping);
	}
}

bool UPxcBlueprintLibrary::Key_GetFirstAdaptiveAxMapping(const TArray<FInputAxisKeyMapping>& tarrMappings, FInputAxisKeyMapping& OutMapping)
{
	UPXCycleInstance* pGI = Cast<UPXCycleInstance>(GWorld ? GWorld->GetGameInstance() : nullptr);
	if (!pGI) return false;

	for (const FInputAxisKeyMapping& Mapping : tarrMappings)
	{
		bool bCurKb = UKismetInputLibrary::Key_IsKeyboardKey(Mapping.Key);
		if (pGI->m_bKeyboardRuntime ? bCurKb : !bCurKb)
		{
			OutMapping = Mapping;
			return true;
		}
	}
	return false;
}

bool UPxcBlueprintLibrary::Key_GetIndexedDevisedAxMapping(const TArray<FInputAxisKeyMapping>& tarrMappings,
	bool bDevisedKeyboard, int32 iIndex, FInputAxisKeyMapping& OutMapping)
{
	int32 i = 0;
	for (const FInputAxisKeyMapping& Mapping : tarrMappings)
	{
		bool bCurKb = UKismetInputLibrary::Key_IsKeyboardKey(Mapping.Key);
		if (bDevisedKeyboard ? bCurKb : !bCurKb)
		{
			if (i == iIndex)
			{
				OutMapping = Mapping;
				return true;
			}
			i++;
		}
	}
	return false;
}

FPxcInputMapping UPxcBlueprintLibrary::Key_MakeInputMapping(const FName& ActionName)
{
	return FPxcInputMapping(ActionName);
}

inline FName GetFirstGamepadModifierName(const FName& ModiActionName)
{
	TArray<FInputActionKeyMapping> tarrMappings;
	TArray<FGamepadCombineMapping> tarrGamepadCombMappings;
	UPxcBlueprintLibrary::Key_GetAllActionMappingByName(ModiActionName, tarrMappings, tarrGamepadCombMappings);
	if (tarrMappings.Num() > 0 || tarrGamepadCombMappings.Num() > 0)
	{
		FInputActionKeyMapping FirstMapping;
		FGamepadCombineMapping FirstCombineMapping;
		int32 iResult = UPxcBlueprintLibrary::Key_GetIndexedDevisedAcMapping(tarrMappings, tarrGamepadCombMappings,
			false, 0, false, FirstMapping, FirstCombineMapping);
		if (iResult)
			return FirstMapping.Key.GetFName();
	}
	return FName();
}

UMaterialInstance* UPxcBlueprintLibrary::Key_ParseIconFromInputMapping(const FPxcInputMapping& InputMapping, int32 iModifierNum,
	TArray<UMaterialInstance*>& tarrOutModifierIcons)
{
	UMaterialInstance* pRetMtlInst = nullptr;
	if (InputMapping.KeyName.IsNone())
		return pRetMtlInst;

	CInputKeyRow* pRow = COtherTableCenter::GetInstance()->GetInputKeyRowByName(TCHAR_TO_ANSI(*InputMapping.KeyName.ToString()));
	if (pRow)
	{
		const FString* pIconPath = GetDefault<UPxcGameConfig>()->tmapDynamicAssetsPathes.Find(EDynamicAssetsType::InputKeyIcon);
		check(pIconPath);
		FString sMainFile = ANSI_TO_TCHAR(pRow->m_strMtlInstFile.c_str());
		FString&& sMainPath = FString::Printf(TEXT(UASSETREF_OBJECT), TEXT(UASSETREFHEAD_MTLINST), **pIconPath, *sMainFile, *sMainFile);
		pRetMtlInst = LoadObject<UMaterialInstance>(nullptr, *sMainPath);

		if (ensureAlways(pRetMtlInst) && InputMapping.uModifierCode != 0)
		{
			static TArray<FName> tarrModifierNames = {
				TEXT("Shift"), TEXT("Ctrl"), TEXT("Alt"), TEXT("Cmd")//, TEXT("")
			};
			static TSet<int32> tsetGamepadCombIndex = { 4 };

			for (int32 i = 0; i < tarrModifierNames.Num(); ++i)
			{
				if (!(InputMapping.uModifierCode & 1 << i))
					continue;

				FName ModiKeyName = tarrModifierNames[i];
				if (tsetGamepadCombIndex.Contains(i))
					ModiKeyName = GetFirstGamepadModifierName(ModiKeyName);
				CInputKeyRow* pModiRow = COtherTableCenter::GetInstance()->GetInputKeyRowByName(TCHAR_TO_ANSI(*ModiKeyName.ToString()));
				if (pModiRow)
				{
					FString sModiFile = ANSI_TO_TCHAR(pModiRow->m_strMtlInstFile.c_str());
					FString&& sModiPath = FString::Printf(TEXT(UASSETREF_OBJECT), TEXT(UASSETREFHEAD_MTLINST), **pIconPath, *sModiFile, *sModiFile);
					UMaterialInstance* pModiMtlInst = LoadObject<UMaterialInstance>(nullptr, *sModiPath);
					tarrOutModifierIcons.Add(pModiMtlInst);
					if (tarrOutModifierIcons.Num() >= iModifierNum) break;
				}
			}
		}
	}

	return pRetMtlInst;
}

FPxcAxputMapping UPxcBlueprintLibrary::Key_MakeAxputMapping(const FName& AxisName, bool bPositiveDir)
{
	return FPxcAxputMapping(AxisName, bPositiveDir);
}

inline FString LinkFloatAxisKeyName(const FName& KeyName, bool bFloatPositive)
{
	return (KeyName.ToString() + (bFloatPositive ? TEXT("1") : TEXT("0")));
}

UMaterialInstance* UPxcBlueprintLibrary::Key_ParseIconFromAxputMapping(const FPxcAxputMapping& AxputMapping)
{
	UMaterialInstance* pRetMtlInst = nullptr;
	if (AxputMapping.KeyName.IsNone())
		return pRetMtlInst;

	CInputKeyRow* pRow = COtherTableCenter::GetInstance()->GetInputKeyRowByName(TCHAR_TO_ANSI(*AxputMapping.KeyName.ToString()));
	if (!pRow)
	{
		FString&& sLinkedKeyName = LinkFloatAxisKeyName(AxputMapping.KeyName, AxputMapping.iFloatPositive > 0);
		pRow = COtherTableCenter::GetInstance()->GetInputKeyRowByName(TCHAR_TO_ANSI(*sLinkedKeyName));
	}

	if (pRow)
	{
		const FString* pIconPath = GetDefault<UPxcGameConfig>()->tmapDynamicAssetsPathes.Find(EDynamicAssetsType::InputKeyIcon);
		check(pIconPath);
		FString sIconFile = ANSI_TO_TCHAR(pRow->m_strMtlInstFile.c_str());
		FString&& sIconPath = FString::Printf(TEXT(UASSETREF_OBJECT), TEXT(UASSETREFHEAD_MTLINST), **pIconPath, *sIconFile, *sIconFile);
		pRetMtlInst = LoadObject<UMaterialInstance>(nullptr, *sIconPath);
	}
	return pRetMtlInst;
}

float UPxcBlueprintLibrary::Math_MagClamp(float fValue, float fMin, float fMax, bool bMidAdsorption, bool bMidToMax, float fPrecision)
{
	fValue = FMath::Clamp(fValue, fMin, fMax);
	float fMid = (fMin + fMax) / 2.0f;
	if (FMath::Abs(fValue - fMid) > fPrecision)
		return (fValue > fMid ? fMax : fMin);
	else if (bMidAdsorption)
		return (bMidToMax ? fMax : fMin);
	return fValue;
}