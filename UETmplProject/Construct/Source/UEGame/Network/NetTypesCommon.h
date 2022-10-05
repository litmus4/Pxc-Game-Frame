// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <functional>
#include <list>
#include "NetTypesCommon.generated.h"

/**
 * 
 */
UCLASS()
class UEGAME_API UNetTypesCommon : public UObject
{
	GENERATED_BODY()
	
};

//USTRUCT()
//struct FNetHashFastArrayHelper : public FFastArraySerializer//FLAGJK 编译错误
//{
//	GENERATED_BODY()
//public:
//	virtual ~FNetHashFastArrayHelper() {}
//
//	UPROPERTY()
//	TArray<int32> Indexes;
//
//	UPROPERTY()
//	TArray<int32> RemovedIndexes;
//
//	UPROPERTY()
//	bool bRealRemoval = false;
//
//	UPROPERTY()
//	bool bNoArrayToHash = false;
//
//	std::list<TSharedPtr<struct NetQueuedOpBase>> Queue;
//
//	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms);
//	virtual bool CommonDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms) { return false; }
//	virtual void ArrayRemoveAt(int32 Index) {}
//
//	template<typename ItemType, typename OpType>
//	void PushAddOrModifyOp(const std::function<bool(const ItemType&)>& Predicate,
//		const std::function<ItemType()>& NewFunc, const std::function<void(ItemType&)>& ModiFunc)
//	{
//		TSharedPtr<OpType> Op = MakeShared<OpType>();
//		Op->Predicate = Predicate;
//		Op->NewFunc = NewFunc;
//		Op->ModiFunc = ModiFunc;
//		Queue.push_back(Op);
//	}
//
//	template<typename ItemType, typename OpType>
//	void PushAddOp(const ItemType& NewItem)
//	{
//		TSharedPtr<OpType> Op = MakeShared<OpType>();
//		Op->NewItem = NewItem;
//		Queue.push_back(Op);
//	}
//
//	template<typename ItemType, typename OpType>
//	void PushModifyOp(const std::function<bool(const ItemType&)>& Predicate, const std::function<void(ItemType&)>& ModiFunc)
//	{
//		TSharedPtr<OpType> Op = MakeShared<OpType>();
//		Op->Predicate = Predicate;
//		Op->ModiFunc = ModiFunc;
//		Queue.push_back(Op);
//	}
//
//	template<typename ItemType, typename OpType>
//	void PushRemoveOp(const std::function<bool(const ItemType&)>& Predicate)
//	{
//		TSharedPtr<OpType> Op = MakeShared<OpType>();
//		Op->Predicate = Predicate;
//		Queue.push_back(Op);
//	}
//
//	template<typename KeyType, typename ValueType, typename ItemType, typename OpType>
//	void PushReplaceMapOp(const TMap<KeyType, ValueType>& Map, const std::function<ItemType(const TPair<KeyType, ValueType>&)>& MakeFunc)
//	{
//		TSharedPtr<OpType> Op = MakeShared<OpType>();
//		Op->Hash = Map;
//		Op->MakeFunc = MakeFunc;
//		Queue.push_back(Op);
//	}
//
//	template<typename ElemType, typename ItemType, typename OpType>
//	void PushReplaceSetOp(const TSet<ElemType>& Set, const std::function<ItemType(const ElemType&)>& MakeFunc)
//	{
//		TSharedPtr<OpType> Op = MakeShared<OpType>();
//		Op->Hash = Set;
//		Op->MakeFunc = MakeFunc;
//		Queue.push_back(Op);
//	}
//
//	template<typename OpType>
//	void PushEmptyOp()
//	{
//		TSharedPtr<OpType> Op = MakeShared<OpType>();
//		Queue.push_back(Op);
//	}
//
//	virtual void PopSeveralOp() {}
//};
//
//struct NetQueuedOpBase
//{
//	enum Type
//	{
//		Invalid = 0,
//		AddOrModify,
//		Add,
//		Modify,
//		Remove,
//		Replace,
//		Empty,
//	};
//
//	virtual ~NetQueuedOpBase() {}
//	virtual Type GetType() { return Type::Invalid; }
//};
//
//USTRUCT()
//struct FNetIdentifiedPair
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY()
//	int32 Uid;
//
//	UPROPERTY()
//	float Value;
//};
//
//USTRUCT()
//struct FNetIdentifiedStack
//{
//	GENERATED_BODY()
//public:
//	FNetIdentifiedStack() = default;
//	//FNetIdentifiedStack(const struct FIdentifiedLessStack& LessStack) { FromLogicStack(LessStack); }
//	//FNetIdentifiedStack(const struct FIdentifiedGreaterStack& GreaterStack) { FromLogicStack(GreaterStack); }
//
//	UPROPERTY()
//	TArray<FNetIdentifiedPair> Stack;
//
//	template<typename T>
//	void FromLogicStack(const T& InStack)
//	{
//		Stack.Empty();
//		for (auto& Pair : InStack.tmapUidToIt)
//		{
//			FNetIdentifiedPair NetPair;
//			NetPair.Uid = Pair.Key;
//			NetPair.Value = *Pair.Value;
//			Stack.Add(NetPair);
//		}
//	}
//
//	template<typename T>
//	T ToLogicStack() const
//	{
//		T RetStack;
//		for (const FNetIdentifiedPair& Pair : Stack)
//		{
//			auto RetPair = RetStack.set.insert(Pair.Value);
//			RetStack.tmapUidToIt.Add(Pair.Uid, RetPair.first);
//		}
//		return RetStack;
//	}
//};
//
////===================TMap/TSet <-> FastArray 复制用函数声明与定义宏===================
//
////-----AddOrModify-----
//
//#define DECLARE_ADDORMODIFY_NET_FUNC(MainName) static void AddOrModifyNet##MainName##Array(\
//	FNet##MainName##Array& NetArray, std::function<bool(const FNet##MainName&)> Predicate,\
//	std::function<FNet##MainName()> NewFunc, std::function<void(FNet##MainName&)> ModiFunc)
//
//#define ADDMOD_NET_MAIN_CODEBLOCK(MainName) int32 Index = NetArray.Array.IndexOfByPredicate(Predicate);\
//	if (Index != INDEX_NONE)\
//	{\
//		FNet##MainName& Item = NetArray.Array[Index];\
//		ModiFunc(Item);\
//		NetArray.MarkItemDirty(Item);\
//	}\
//	else\
//	{\
//		Index = NetArray.Array.Add(NewFunc());\
//		NetArray.MarkItemDirty(NetArray.Array[Index]);\
//	}\
//	NetArray.Indexes.AddUnique(Index);
//
//#define ADDMOD_NET_QUEUE_CODEBLOCK(MainName) if (NetArray.bRealRemoval)\
//	{\
//		NetArray.PushAddOrModifyOp<FNet##MainName, FNet##MainName##Array::Net##MainName##AddModOp>(Predicate, NewFunc, ModiFunc);\
//		return;\
//	}
//
//#define DEFINE_ADDORMODIFY_NET_FUNC(ClassName, MainName) void ClassName::AddOrModifyNet##MainName##Array(\
//	FNet##MainName##Array& NetArray, std::function<bool(const FNet##MainName&)> Predicate,\
//	std::function<FNet##MainName()> NewFunc, std::function<void(FNet##MainName&)> ModiFunc)\
//{\
//	ADDMOD_NET_MAIN_CODEBLOCK(MainName)\
//}
//
//#define DEFINE_ADDORMODIFY_NET_FUNC_Q(ClassName, MainName) void ClassName::AddOrModifyNet##MainName##Array(\
//	FNet##MainName##Array& NetArray, std::function<bool(const FNet##MainName&)> Predicate,\
//	std::function<FNet##MainName()> NewFunc, std::function<void(FNet##MainName&)> ModiFunc)\
//{\
//	ADDMOD_NET_QUEUE_CODEBLOCK(MainName)\
//	ADDMOD_NET_MAIN_CODEBLOCK(MainName)\
//}
//
////-----Add-----
//
//#define DECLARE_ADD_NET_FUNC(MainName) static void AddNet##MainName##Array(FNet##MainName##Array& NetArray, const FNet##MainName& NewItem)
//
//#define ADD_NET_MAIN_CODEBLOCK int32 Index = NetArray.Array.Add(NewItem);\
//	NetArray.MarkItemDirty(NetArray.Array[Index]);\
//	NetArray.Indexes.AddUnique(Index);
//
//#define ADD_NET_QUEUE_CODEBLOCK(MainName) if (NetArray.bRealRemoval)\
//	{\
//		NetArray.PushAddOp<FNet##MainName, FNet##MainName##Array::Net##MainName##AddOp>(NewItem);\
//		return;\
//	}
//
//#define DEFINE_ADD_NET_FUNC(ClassName, MainName) void ClassName::AddNet##MainName##Array(FNet##MainName##Array& NetArray,\
//	const FNet##MainName& NewItem)\
//{\
//	ADD_NET_MAIN_CODEBLOCK\
//}
//
//#define DEFINE_ADD_NET_FUNC_Q(ClassName, MainName) void ClassName::AddNet##MainName##Array(FNet##MainName##Array& NetArray,\
//	const FNet##MainName& NewItem)\
//{\
//	ADD_NET_QUEUE_CODEBLOCK(MainName)\
//	ADD_NET_MAIN_CODEBLOCK\
//}
//
////-----Modify-----
//
//#define DECLARE_MODIFY_NET_FUNC(MainName) static void ModifyNet##MainName##Array(FNet##MainName##Array& NetArray,\
//	std::function<bool(const FNet##MainName&)> Predicate, std::function<void(FNet##MainName##&)> ModiFunc)
//
//#define MOD_NET_MAIN_CODEBLOCK(MainName) int32 Index = NetArray.Array.IndexOfByPredicate(Predicate);\
//	if (Index != INDEX_NONE)\
//	{\
//		FNet##MainName& Item = NetArray.Array[Index];\
//		ModiFunc(Item);\
//		NetArray.MarkItemDirty(Item);\
//		NetArray.Indexes.AddUnique(Index);\
//	}
//
//#define MOD_NET_QUEUE_CODEBLOCK(MainName) if (NetArray.bRealRemoval)\
//	{\
//		NetArray.PushModifyOp<FNet##MainName, FNet##MainName##Array::Net##MainName##ModOp>(Predicate, ModiFunc);\
//		return;\
//	}
//
//#define DEFINE_MODIFY_NET_FUNC(ClassName, MainName) void ClassName::ModifyNet##MainName##Array(FNet##MainName##Array& NetArray,\
//	std::function<bool(const FNet##MainName&)> Predicate, std::function<void(FNet##MainName&)> ModiFunc)\
//{\
//	MOD_NET_MAIN_CODEBLOCK(MainName)\
//}
//
//#define DEFINE_MODIFY_NET_FUNC_Q(ClassName, MainName) void ClassName::ModifyNet##MainName##Array(FNet##MainName##Array& NetArray,\
//	std::function<bool(const FNet##MainName&)> Predicate, std::function<void(FNet##MainName&)> ModiFunc)\
//{\
//	MOD_NET_QUEUE_CODEBLOCK(MainName)\
//	MOD_NET_MAIN_CODEBLOCK(MainName)\
//}
//
////-----Remove-----
////有Remove函数时可能需要排队，参数中lambda捕获列表必须传值
////无Remove函数时无排队，参数中lambda捕获列表可传引用/指针提升性能
//
//#define DECLARE_REMOVE_NET_FUNC(MainName) static void RemoveNet##MainName##Array(\
//	FNet##MainName##Array& NetArray, std::function<bool(const FNet##MainName&)> Predicate)
//
//#define REM_NET_MAIN_CODEBLOCK int32 Index = NetArray.Array.IndexOfByPredicate(Predicate);\
//	if (Index != INDEX_NONE)\
//	{\
//		NetArray.MarkItemDirty(NetArray.Array[Index]);\
//		NetArray.RemovedIndexes.AddUnique(Index);\
//	}
//
//#define REM_NET_QUEUE_CODEBLOCK(MainName) if (NetArray.bRealRemoval)\
//	{\
//		NetArray.PushRemoveOp<FNet##MainName, FNet##MainName##Array::Net##MainName##RemOp>(Predicate);\
//		return;\
//	}
//
//#define DEFINE_REMOVE_NET_FUNC(ClassName, MainName) void ClassName::RemoveNet##MainName##Array(\
//	FNet##MainName##Array& NetArray, std::function<bool(const FNet##MainName&)> Predicate)\
//{\
//	REM_NET_MAIN_CODEBLOCK\
//}
//
//#define DEFINE_REMOVE_NET_FUNC_Q(ClassName, MainName) void ClassName::RemoveNet##MainName##Array(\
//	FNet##MainName##Array& NetArray, std::function<bool(const FNet##MainName&)> Predicate)\
//{\
//	REM_NET_QUEUE_CODEBLOCK(MainName)\
//	REM_NET_MAIN_CODEBLOCK\
//}
//
////-----Replace-----
////Replace函数中也有Remove操作，因此参数中lambda捕获列表也必须传值
//
//#define DECLARE_REPLACE_MAP_NET_FUNC(MainName, KeyClass, ValueClass) static void ReplaceNet##MainName##Array(\
//	FNet##MainName##Array& NetArray, TMap<KeyClass, ValueClass>& Hash,\
//	std::function<FNet##MainName(const TPair<KeyClass, ValueClass>&)> MakeFunc)
//
//#define DECLARE_REPLACE_SET_NET_FUNC(MainName, ElemClass) static void ReplaceNet##MainName##Array(\
//	FNet##MainName##Array& NetArray, TSet<ElemClass>& Hash, std::function<FNet##MainName(const ElemClass&)> MakeFunc)
//
//#define REPL_NET_MAIN_CODEBLOCK for (int32 i = 0; i < NetArray.Array.Num(); ++i)\
//	{\
//		NetArray.MarkItemDirty(NetArray.Array[i]);\
//		NetArray.RemovedIndexes.AddUnique(i);\
//	}\
//	for (auto& PairOrElem : Hash)\
//	{\
//		int32 Index = NetArray.Array.Add(MakeFunc(PairOrElem));\
//		NetArray.MarkItemDirty(NetArray.Array[Index]);\
//		NetArray.Indexes.AddUnique(Index);\
//	}
//
//#define REPLMAP_NET_QUEUE_CODEBLOCK(MainName, KeyClass, ValueClass) if (NetArray.bRealRemoval)\
//	{\
//		NetArray.PushReplaceMapOp<KeyClass, ValueClass, FNet##MainName,\
//			FNet##MainName##Array::Net##MainName##ReplOp>(Hash, MakeFunc);\
//		return;\
//	}
//
//#define REPLSET_NET_QUEUE_CODEBLOCK(MainName, ElemClass) if (NetArray.bRealRemoval)\
//	{\
//		NetArray.PushReplaceSetOp<ElemClass, FNet##MainName,\
//			FNet##MainName##Array::Net##MainName##ReplOp>(Hash, MakeFunc);\
//		return;\
//	}
//
//#define DEFINE_REPLACE_MAP_NET_FUNC(ClassName, MainName, KeyClass, ValueClass) void ClassName::ReplaceNet##MainName##Array(\
//	FNet##MainName##Array& NetArray, TMap<KeyClass, ValueClass>& Hash,\
//	std::function<FNet##MainName(const TPair<KeyClass, ValueClass>&)> MakeFunc)\
//{\
//	REPL_NET_MAIN_CODEBLOCK\
//}
//
//#define DEFINE_REPLACE_MAP_NET_FUNC_Q(ClassName, MainName, KeyClass, ValueClass) void ClassName::ReplaceNet##MainName##Array(\
//	FNet##MainName##Array& NetArray, TMap<KeyClass, ValueClass>& Hash,\
//	std::function<FNet##MainName(const TPair<KeyClass, ValueClass>&)> MakeFunc)\
//{\
//	REPLMAP_NET_QUEUE_CODEBLOCK(MainName, KeyClass, ValueClass)\
//	REPL_NET_MAIN_CODEBLOCK\
//}
//
//#define DEFINE_REPLACE_SET_NET_FUNC(ClassName, MainName, ElemClass) void ClassName::ReplaceNet##MainName##Array(\
//	FNet##MainName##Array& NetArray, TSet<ElemClass>& Hash, std::function<FNet##MainName(const ElemClass&)> MakeFunc)\
//{\
//	REPL_NET_MAIN_CODEBLOCK\
//}
//
//#define DEFINE_REPLACE_SET_NET_FUNC_Q(ClassName, MainName, ElemClass) void ClassName::ReplaceNet##MainName##Array(\
//	FNet##MainName##Array& NetArray, TSet<ElemClass>& Hash, std::function<FNet##MainName(const ElemClass&)> MakeFunc)\
//{\
//	REPLSET_NET_QUEUE_CODEBLOCK(MainName, ElemClass)\
//	REPL_NET_MAIN_CODEBLOCK\
//}
//
////-----Empty-----
////Empty函数中也有Remove操作，但参数中没有lambda
//
//#define DECLARE_EMPTY_NET_FUNC(MainName) static void EmptyNet##MainName##Array(FNet##MainName##Array& NetArray)
//
//#define EMP_NET_MAIN_CODEBLOCK for (int32 i = 0; i < NetArray.Array.Num(); ++i)\
//	{\
//		NetArray.MarkItemDirty(NetArray.Array[i]);\
//		NetArray.RemovedIndexes.AddUnique(i);\
//	}
//
//#define EMP_NET_QUEUE_CODEBLOCK(MainName) if (NetArray.bRealRemoval)\
//	{\
//		NetArray.PushEmptyOp<FNet##MainName##Array::Net##MainName##EmpOp>();\
//		return;\
//	}
//
//#define DEFINE_EMPTY_NET_FUNC(ClassName, MainName) void ClassName::EmptyNet##MainName##Array(FNet##MainName##Array & NetArray)\
//{\
//	EMP_NET_MAIN_CODEBLOCK\
//}
//
//#define DEFINE_EMPTY_NET_FUNC_Q(ClassName, MainName) void ClassName::EmptyNet##MainName##Array(FNet##MainName##Array & NetArray)\
//{\
//	EMP_NET_QUEUE_CODEBLOCK(MainName)\
//	EMP_NET_MAIN_CODEBLOCK\
//}
//
////-----OnRep:ArrayToMap-----
//
//#define DECLARE_REPNET_ARRAYTOMAP_FUNC(MainName, KeyClass, ValueClass) static void RepNet##MainName##ArrayToMap(\
//	FNet##MainName##Array& NetArray, TMap<KeyClass, ValueClass>& Map,\
//	std::function<ValueClass(const FNet##MainName&)> NewFunc, std::function<void(ValueClass&, const FNet##MainName&)> ModiFunc)
//
//#define DEFINE_REPNET_ARRAYTOMAP_FUNC(ClassName, MainName, KeyClass, ValueClass, KeyProp) void ClassName::RepNet##MainName##ArrayToMap(\
//	FNet##MainName##Array& NetArray, TMap<KeyClass, ValueClass>& Map,\
//	std::function<ValueClass(const FNet##MainName&)> NewFunc, std::function<void(ValueClass&, const FNet##MainName&)> ModiFunc)\
//{\
//	if (NetArray.bRealRemoval)\
//	{\
//		NetArray.bRealRemoval = false;\
//		return;\
//	}\
//	for (int32 Index : NetArray.Indexes)\
//	{\
//		FNet##MainName& NetItem = NetArray.Array[Index];\
//		ValueClass* Item = Map.Find(NetItem.KeyProp);\
//		if (Item)\
//			ModiFunc(*Item, NetItem);\
//		else\
//			Map.Add(NetItem.KeyProp, NewFunc(NetItem));\
//	}\
//	for (int32 Index : NetArray.RemovedIndexes)\
//	{\
//		FNet##MainName& NetItem = NetArray.Array[Index];\
//		Map.Remove(NetItem.KeyProp);\
//	}\
//	NetArray.Indexes.Empty();\
//	NetArray.RemovedIndexes.Empty();\
//}
//
//#define DECLARE_REPNET_ARRAYTOMAP_FUNC_KF(MainName, KeyClass, ValueClass) static void RepNet##MainName##ArrayToMap(\
//	FNet##MainName##Array& NetArray, TMap<KeyClass, ValueClass>& Map, std::function<KeyClass(const FNet##MainName&)> KeyFunc,\
//	std::function<ValueClass(const FNet##MainName&)> NewFunc, std::function<void(ValueClass&, const FNet##MainName&)> ModiFunc)
//
//#define DEFINE_REPNET_ARRAYTOMAP_FUNC_KF(ClassName, MainName, KeyClass, ValueClass) void ClassName::RepNet##MainName##ArrayToMap(\
//	FNet##MainName##Array& NetArray, TMap<KeyClass, ValueClass>& Map, std::function<KeyClass(const FNet##MainName&)> KeyFunc,\
//	std::function<ValueClass(const FNet##MainName&)> NewFunc, std::function<void(ValueClass&, const FNet##MainName&)> ModiFunc)\
//{\
//	if (NetArray.bRealRemoval)\
//	{\
//		NetArray.bRealRemoval = false;\
//		return;\
//	}\
//	for (int32 Index : NetArray.Indexes)\
//	{\
//		FNet##MainName& NetItem = NetArray.Array[Index];\
//		ValueClass* Item = Map.Find(KeyFunc(NetItem));\
//		if (Item)\
//			ModiFunc(*Item, NetItem);\
//		else\
//			Map.Add(KeyFunc(NetItem), NewFunc(NetItem));\
//	}\
//	for (int32 Index : NetArray.RemovedIndexes)\
//	{\
//		FNet##MainName& NetItem = NetArray.Array[Index];\
//		Map.Remove(KeyFunc(NetItem));\
//	}\
//	NetArray.Indexes.Empty();\
//	NetArray.RemovedIndexes.Empty();\
//}
//
//#define DEFINE_REPNET_ARRAYTOMAP_FUNC_VT(MainName, KeyClass, KeyProp) template<typename ValueType>\
//static void RepNet##MainName##ArrayToMap(FNet##MainName##Array& NetArray, TMap<KeyClass, ValueType>& Map,\
//	std::function<ValueType(const FNet##MainName&)> NewFunc, std::function<void(ValueType&, const FNet##MainName&)> ModiFunc)\
//{\
//	if (NetArray.bRealRemoval)\
//	{\
//		NetArray.bRealRemoval = false;\
//		return;\
//	}\
//	for (int32 Index : NetArray.Indexes)\
//	{\
//		FNet##MainName& NetItem = NetArray.Array[Index];\
//		ValueType* Item = Map.Find(NetItem.KeyProp);\
//		if (Item)\
//			ModiFunc(*Item, NetItem);\
//		else\
//			Map.Add(NetItem.KeyProp, NewFunc(NetItem));\
//	}\
//	for (int32 Index : NetArray.RemovedIndexes)\
//	{\
//		FNet##MainName##& NetItem = NetArray.Array[Index];\
//		Map.Remove(NetItem.KeyProp);\
//	}\
//	NetArray.Indexes.Empty();\
//	NetArray.RemovedIndexes.Empty();\
//}
//
//#define DEFINE_REPNET_ARRAYTOMAP_FUNC_KFVT(MainName, KeyClass) template<typename ValueType> static void RepNet##MainName##ArrayToMap(\
//	FNet##MainName##Array& NetArray, TMap<KeyClass, ValueType>& Map, std::function<KeyClass(const FNet##MainName&)> KeyFunc,\
//	std::function<ValueType(const FNet##MainName&)> NewFunc, std::function<void(ValueType&, const FNet##MainName&)> ModiFunc)\
//{\
//	if (NetArray.bRealRemoval)\
//	{\
//		NetArray.bRealRemoval = false;\
//		return;\
//	}\
//	for (int32 Index : NetArray.Indexes)\
//	{\
//		FNet##MainName& NetItem = NetArray.Array[Index];\
//		ValueType* Item = Map.Find(KeyFunc(NetItem));\
//		if (Item)\
//			ModiFunc(*Item, NetItem);\
//		else\
//			Map.Add(KeyFunc(NetItem), NewFunc(NetItem));\
//	}\
//	for (int32 Index : NetArray.RemovedIndexes)\
//	{\
//		FNet##MainName##& NetItem = NetArray.Array[Index];\
//		Map.Remove(KeyFunc(NetItem));\
//	}\
//	NetArray.Indexes.Empty();\
//	NetArray.RemovedIndexes.Empty();\
//}
//
////-----OnRep:ArrayToSet-----
//
//#define DECLARE_REPNET_ARRAYTOSET_FUNC(MainName, ElemClass) static void RepNet##MainName##ArrayToSet(\
//	FNet##MainName##Array& NetArray, TSet<ElemClass>& Set, std::function<void(ElemClass&, const FNet##MainName&)> ModiFunc)
//
//#define DEFINE_REPNET_ARRAYTOSET_FUNC(ClassName, MainName, ElemClass, ElemProp) void ClassName::RepNet##MainName##ArrayToSet(\
//	FNet##MainName##Array& NetArray, TSet<ElemClass>& Set, std::function<void(ElemClass&, const FNet##MainName&)> ModiFunc)\
//{\
//	if (NetArray.bRealRemoval)\
//	{\
//		NetArray.bRealRemoval = false;\
//		return;\
//	}\
//	for (int32 Index : NetArray.Indexes)\
//	{\
//		FNet##MainName& NetItem = NetArray.Array[Index];\
//		ElemClass* Item = Set.Find(NetItem.ElemProp);\
//		if (Item)\
//			ModiFunc(*Item, NetItem);\
//		else\
//			Set.Add(NetItem.ElemProp);\
//	}\
//	for (int32 Index : NetArray.RemovedIndexes)\
//	{\
//		FNet##MainName& NetItem = NetArray.Array[Index];\
//		Set.Remove(NetItem.ElemProp);\
//	}\
//	NetArray.Indexes.Empty();\
//	NetArray.RemovedIndexes.Empty();\
//}
//
//#define DECLARE_REPNET_ARRAYTOSET_FUNC_EF(MainName, ElemClass) static void RepNet##MainName##ArrayToSet(\
//	FNet##MainName##Array& NetArray, TSet<ElemClass>& Set, std::function<ElemClass(const FNet##MainName&)> ElemFunc,\
//	std::function<void(ElemClass&, const FNet##MainName&)> ModiFunc)
//
//#define DEFINE_REPNET_ARRAYTOSET_FUNC_EF(ClassName, MainName, ElemClass) void ClassName::RepNet##MainName##ArrayToSet(\
//	FNet##MainName##Array& NetArray, TSet<ElemClass>& Set, std::function<ElemClass(const FNet##MainName&)> ElemFunc,\
//	std::function<void(ElemClass&, const FNet##MainName&)> ModiFunc)\
//{\
//	if (NetArray.bRealRemoval)\
//	{\
//		NetArray.bRealRemoval = false;\
//		return;\
//	}\
//	for (int32 Index : NetArray.Indexes)\
//	{\
//		FNet##MainName& NetItem = NetArray.Array[Index];\
//		ElemClass* Item = Set.Find(ElemFunc(NetItem));\
//		if (Item)\
//			ModiFunc(*Item, NetItem);\
//		else\
//			Set.Add(ElemFunc(NetItem));\
//	}\
//	for (int32 Index : NetArray.RemovedIndexes)\
//	{\
//		FNet##MainName& NetItem = NetArray.Array[Index];\
//		Set.Remove(ElemFunc(NetItem));\
//	}\
//	NetArray.Indexes.Empty();\
//	NetArray.RemovedIndexes.Empty();\
//}
//
////===================RealRemoval时排队功能宏===================
//
////-----操作结构体定义宏-----
//
//#define DEFINE_NET_ADDORMODIFY_OP(MainName) struct Net##MainName##AddModOp : public NetQueuedOpBase\
//{\
//	virtual ~Net##MainName##AddModOp() {}\
//	std::function<bool(const FNet##MainName&)> Predicate;\
//	std::function<FNet##MainName()> NewFunc;\
//	std::function<void(FNet##MainName&)> ModiFunc;\
//	virtual Type GetType() override { return Type::AddOrModify; }\
//}
//
//#define DEFINE_NET_ADD_OP(MainName) struct Net##MainName##AddOp : public NetQueuedOpBase\
//{\
//	virtual ~Net##MainName##AddOp() {}\
//	FNet##MainName NewItem;\
//	virtual Type GetType() override { return Type::Add; }\
//}
//
//#define DEFINE_NET_MODIFY_OP(MainName) struct Net##MainName##ModOp : public NetQueuedOpBase\
//{\
//	virtual ~Net##MainName##ModOp() {}\
//	std::function<bool(const FNet##MainName&)> Predicate;\
//	std::function<void(FNet##MainName&)> ModiFunc;\
//	virtual Type GetType() override { return Type::Modify; }\
//}
//
//#define DEFINE_NET_REMOVE_OP(MainName) struct Net##MainName##RemOp : public NetQueuedOpBase\
//{\
//	virtual ~Net##MainName##RemOp() {}\
//	std::function<bool(const FNet##MainName&)> Predicate;\
//	virtual Type GetType() override { return Type::Remove; }\
//}
//
//#define DEFINE_NET_REPLACE_MAP_OP(MainName, KeyClass, ValueClass) struct Net##MainName##ReplOp : public NetQueuedOpBase\
//{\
//	virtual ~Net##MainName##ReplOp() {}\
//	TMap<KeyClass, ValueClass> Hash;\
//	std::function<FNet##MainName(const TPair<KeyClass, ValueClass>&)> MakeFunc;\
//	virtual Type GetType() override { return Type::Replace; }\
//}
//
//#define DEFINE_NET_REPLACE_SET_OP(MainName, ElemClass) struct Net##MainName##ReplOp : public NetQueuedOpBase\
//{\
//	virtual ~Net##MainName##ReplOp() {}\
//	TSet<ElemClass> Hash;\
//	std::function<FNet##MainName(const ElemClass&)> MakeFunc;\
//	virtual Type GetType() override { return Type::Replace; }\
//}
//
//#define DEFINE_NET_EMPTY_OP(MainName) struct Net##MainName##EmpOp : public NetQueuedOpBase\
//{\
//	virtual ~Net##MainName##EmpOp() {}\
//	virtual Type GetType() override { return Type::Empty; }\
//}
//
////-----操作出队处理宏-----
//
//#define BEGIN_NET_POP_SEVERAL_OP if (Queue.empty()) return;\
//	NetQueuedOpBase::Type OpType = NetQueuedOpBase::Type::Invalid;\
//	do {\
//		TSharedPtr<NetQueuedOpBase>& Op = Queue.front();\
//		OpType = Op->GetType();\
//		switch (OpType)\
//		{
//
//#define CASE_NET_OPTYPE_ADDORMODIFY(ClassName, MainName) case NetQueuedOpBase::Type::AddOrModify:\
//		{\
//			auto DerivedOp = static_cast<Net##MainName##AddModOp*>(Op.Get());\
//			ClassName::AddOrModifyNet##MainName##Array(*this, DerivedOp->Predicate, DerivedOp->NewFunc, DerivedOp->ModiFunc);\
//			break;\
//		}
//
//#define CASE_NET_OPTYPE_ADD(ClassName, MainName) case NetQueuedOpBase::Type::Add:\
//		{\
//			auto DerivedOp = static_cast<Net##MainName##AddOp*>(Op.Get());\
//			ClassName::AddNet##MainName##Array(*this, DerivedOp->NewItem);\
//			break;\
//		}
//
//#define CASE_NET_OPTYPE_MODIFY(ClassName, MainName) case NetQueuedOpBase::Type::Modify:\
//		{\
//			auto DerivedOp = static_cast<Net##MainName##ModOp*>(Op.Get());\
//			ClassName::ModifyNet##MainName##Array(*this, DerivedOp->Predicate, DerivedOp->ModiFunc);\
//			break;\
//		}
//
//#define CASE_NET_OPTYPE_REMOVE(ClassName, MainName) case NetQueuedOpBase::Type::Remove:\
//		{\
//			auto DerivedOp = static_cast<Net##MainName##RemOp*>(Op.Get());\
//			ClassName::RemoveNet##MainName##Array(*this, DerivedOp->Predicate);\
//			break;\
//		}
//
//#define CASE_NET_OPTYPE_REPLACE(ClassName, MainName) case NetQueuedOpBase::Type::Replace:\
//		{\
//			auto DerivedOp = static_cast<Net##MainName##ReplOp*>(Op.Get());\
//			ClassName::ReplaceNet##MainName##Array(*this, DerivedOp->Hash, DerivedOp->MakeFunc);\
//			break;\
//		}
//
//#define CASE_NET_OPTYPE_EMPTY(ClassName, MainName) case NetQueuedOpBase::Type::Empty:\
//		{\
//			ClassName::EmptyNet##MainName##Array(*this);\
//			break;\
//		}
//
//#define END_NET_POP_SEVERAL_OP }\
//		Queue.pop_front();\
//	} while (OpType != NetQueuedOpBase::Type::Remove && OpType != NetQueuedOpBase::Type::Replace &&\
//		OpType != NetQueuedOpBase::Type::Empty && !Queue.empty())

//============================================================

DECLARE_DELEGATE_RetVal_OneParam(bool, FNetEachPlayerDelegate, class APxcPlayerCharacter*);
DECLARE_DYNAMIC_DELEGATE_OneParam(FNetDynOnEachPlayerDelegate, class APxcPlayerCharacter*, Player);