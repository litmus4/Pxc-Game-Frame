// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SignatureStructs.generated.h"

UENUM(BlueprintType)
enum class EPtrModelBaseType : uint8
{
	Model = 0,
	PassiveListener,
	NetParamWrapper,
	TestParameter,
};

USTRUCT()
struct FPointerModel
{
	GENERATED_BODY()
public:
	FPointerModel() : lUid(-1LL), bFake(false) {}
	FPointerModel(const int64 xUid)
		: lUid(xUid), bFake(true) {}
	virtual ~FPointerModel() {}

	void GenerateUidLocal();

	friend uint32 GetTypeHash(const FPointerModel& Model);

	virtual UScriptStruct* GetScriptStruct() const
	{
		return FPointerModel::StaticStruct();
	}

	virtual EPtrModelBaseType GetBaseType() const
	{
		return EPtrModelBaseType::Model;
	}

	UPROPERTY()
	int64 lUid;

	UPROPERTY()
	bool bFake;
};

USTRUCT(BlueprintType)
struct FSharedSignature
{
	GENERATED_BODY()
public:
	FSharedSignature() = default;
	FSharedSignature(FPointerModel* xModel);
	FSharedSignature(const FSharedSignature& Other);
	FSharedSignature(const FPointerModel& xModel);

	FORCEINLINE bool IsValid() const { return pModel.IsValid(); }
	FORCEINLINE const FPointerModel* Get() const { return pModel.Get(); }
	FORCEINLINE FPointerModel* Get() { return pModel.Get(); }
	FORCEINLINE TSharedPtr<FPointerModel> GetShared() const { return pModel; }
	FORCEINLINE const FPointerModel& operator*() const { return *pModel; }
	FORCEINLINE FPointerModel& operator*() { return *pModel; }
	FORCEINLINE FPointerModel* operator->() const { return pModel.Get(); }
	bool operator==(const FSharedSignature& Other) const;
	FORCEINLINE bool operator!=(const FSharedSignature& Other) const { return !(*this == Other); }
	bool operator<(const FSharedSignature& Other) const;
	FORCEINLINE void Reset() { pModel.Reset(); }
	FORCEINLINE EPtrModelBaseType GetBaseType() const
	{ return (IsValid() ? pModel->GetBaseType() : EPtrModelBaseType::Model); }
	//EPtrModelBaseType K2_GetBaseType(); TODOJK

	template<class T>
	bool IsDerived() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, FPointerModel>::Value,
			"FSharedSignature IsDerived static_assert");
		return (IsValid() && pModel->GetScriptStruct() == T::StaticStruct());
	}

	template<class T>
	const T* GetDerivedPtr() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, FPointerModel>::Value,
			"FSharedSignature GetDerivedPtr static_assert");
		check(IsValid());
		check(pModel->GetScriptStruct() == T::StaticStruct());
		return static_cast<const T*>(Get());
	}

	template<class T>
	T* GetDerivedPtr()
	{
		static_assert(TPointerIsConvertibleFromTo<T, FPointerModel>::Value,
			"FSharedSignature GetDerivedPtr static_assert");
		check(IsValid());
		check(pModel->GetScriptStruct() == T::StaticStruct());
		return static_cast<T*>(Get());
	}

	friend uint32 GetTypeHash(const FSharedSignature& Sig);

private:
	TSharedPtr<FPointerModel> pModel;
};

USTRUCT(BlueprintType)
struct FSharedSigPure
{
	GENERATED_BODY()
public:
	FSharedSigPure() = default;
	FSharedSigPure(FPointerModel * xModel);
	FSharedSigPure(const FSharedSigPure& Other);
	FSharedSigPure(const FPointerModel& xModel);

	FORCEINLINE bool IsValid() const { return pModel.IsValid(); }
	FORCEINLINE const FPointerModel* Get() const { return pModel.Get(); }
	FORCEINLINE FPointerModel* Get() { return pModel.Get(); }
	FORCEINLINE TSharedPtr<FPointerModel> GetShared() const { return pModel; }
	FORCEINLINE const FPointerModel& operator*() const { return *pModel; }
	FORCEINLINE FPointerModel& operator*() { return *pModel; }
	FORCEINLINE FPointerModel* operator->() const { return pModel.Get(); }
	bool operator==(const FSharedSigPure& Other) const;
	FORCEINLINE bool operator!=(const FSharedSigPure& Other) const { return !(*this == Other); }
	bool operator<(const FSharedSigPure& Other) const;
	FORCEINLINE void Reset() { pModel.Reset(); }
	FORCEINLINE EPtrModelBaseType GetBaseType() const
	{ return (IsValid() ? pModel->GetBaseType() : EPtrModelBaseType::Model); }

	template<class T>
	bool IsDerived() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, FPointerModel>::Value,
			"FSharedSigPure IsDerived static_assert");
		return (IsValid() && pModel->GetScriptStruct() == T::StaticStruct());
	}

	template<class T>
	const T* GetDerivedPtr() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, FPointerModel>::Value,
			"FSharedSigPure GetDerivedPtr static_assert");
		check(IsValid());
		check(pModel->GetScriptStruct() == T::StaticStruct());
		return static_cast<T*>(Get());
	}

	template<class T>
	T* GetDerivedPtr()
	{
		static_assert(TPointerIsConvertibleFromTo<T, FPointerModel>::Value,
			"FSharedSigPure GetDerivedPtr static_assert");
		check(IsValid());
		check(pModel->GetScriptStruct() == T::StaticStruct());
		return static_cast<T*>(Get());
	}

	friend uint32 GetTypeHash(const FSharedSigPure& Sig);

private:
	TSharedPtr<FPointerModel> pModel;
};