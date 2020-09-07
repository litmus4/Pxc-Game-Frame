// Fill out your copyright notice in the Description page of Project Settings.


#include "SignatureStructs.h"
#include "Framework/PxcAssistantSubsystem.h"
#include "PxcUtil/IDPool64.h"

void FPointerModel::GenerateUidLocal()
{
	PxcUtil::CIDPool64* pUidPool =
		UPxcAssistantSubsystem::GetInstance()->GetUidPool64(ASUIDPOOL::PointerModelLL);
	lUid = pUidPool->Generate();
}

uint32 GetTypeHash(const FPointerModel& Model)
{
	return GetTypeHash(Model.lUid);
}

FSharedSignature::FSharedSignature(FPointerModel* xModel)
{
	pModel = TSharedPtr<FPointerModel>(xModel);
}

FSharedSignature::FSharedSignature(const FSharedSignature& Other)
{
	if (Other.IsValid())
	{
		TCheckedObjPtr<UScriptStruct> pScriptStruct = Other->GetScriptStruct();
		check(pScriptStruct.IsValid());

		FPointerModel* pNewModel = (FPointerModel*)FMemory::Malloc(pScriptStruct->GetCppStructOps()->GetSize());
		pScriptStruct->InitializeStruct(pNewModel);
		pScriptStruct->CopyScriptStruct(pNewModel, Other.Get());

		pModel = TSharedPtr<FPointerModel>(pNewModel);
	}
	else
		Reset();

}

FSharedSignature::FSharedSignature(const FPointerModel& xModel)
{
	TCheckedObjPtr<UScriptStruct> pScriptStruct = xModel.GetScriptStruct();
	check(pScriptStruct.IsValid());

	FPointerModel* pNewModel = (FPointerModel*)FMemory::Malloc(pScriptStruct->GetCppStructOps()->GetSize());
	pScriptStruct->InitializeStruct(pNewModel);
	pScriptStruct->CopyScriptStruct(pNewModel, &xModel);

	pModel = TSharedPtr<FPointerModel>(pNewModel);
}

bool FSharedSignature::operator==(const FSharedSignature& Other) const
{
	UScriptStruct* pThisStruct = nullptr;
	bool bThisFake = false; int64 lThisUid = -1LL;
	if (IsValid())
	{
		pThisStruct = pModel->GetScriptStruct();
		bThisFake = pModel->bFake; lThisUid = pModel->lUid;
	}

	UScriptStruct* pOtherStruct = nullptr;
	if (Other.IsValid())
	{
		pOtherStruct = Other->GetScriptStruct();
		if (bThisFake || Other->bFake)
			return lThisUid == Other->lUid;
	}

	if (pThisStruct != pOtherStruct) return false;
	if (!pThisStruct && !pOtherStruct) return true;
	check(pThisStruct && pOtherStruct && pThisStruct == pOtherStruct);
	return pThisStruct->CompareScriptStruct(Get(), Other.Get(), 0);

}

bool FSharedSignature::operator<(const FSharedSignature& Other) const
{
	int64 lThisUid = -1LL; uint64 ulThisAdd = 0;
	if (IsValid())
	{
		lThisUid = pModel->lUid; ulThisAdd = (uint64)Get();
	}

	if (IsValid())
	{
		if (lThisUid != Other->lUid)
			return lThisUid < Other->lUid;
		uint64 ulOtherAdd = (uint64)Other.Get();
		if (ulThisAdd != ulOtherAdd)
			return ulThisAdd < ulOtherAdd;
	}
	return false;
}

uint32 GetTypeHash(const FSharedSignature& Sig)
{
	if (Sig.IsValid())
		return GetTypeHash(*Sig);
	return GetTypeHash(-1LL);

}