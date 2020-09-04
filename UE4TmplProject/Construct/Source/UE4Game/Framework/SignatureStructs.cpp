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

FSharedSignature::FSharedSignature(const FSharedSignature& OtherSig)
{
	if (OtherSig.IsValid())
	{
		TCheckedObjPtr<UScriptStruct> pScriptStruct = OtherSig->GetScriptStruct();
		check(pScriptStruct.IsValid());

		FPointerModel* pNewModel = (FPointerModel*)FMemory::Malloc(pScriptStruct->GetCppStructOps()->GetSize());
		pScriptStruct->InitializeStruct(pNewModel);
		pScriptStruct->CopyScriptStruct(pNewModel, OtherSig.Get());

		pModel = TSharedPtr<FPointerModel>(pNewModel);
	}
	else
		Clear();

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