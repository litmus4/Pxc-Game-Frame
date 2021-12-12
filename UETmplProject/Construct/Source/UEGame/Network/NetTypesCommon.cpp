// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/NetTypesCommon.h"

//bool FNetHashFastArrayHelper::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
//{
//	bool bRet = CommonDeltaSerialize(DeltaParms);
//
//	if (DeltaParms.Writer)
//	{
//		(*DeltaParms.Writer) << Indexes << RemovedIndexes << bRealRemoval << bNoArrayToHash;
//
//		if (!bRealRemoval)
//		{
//			Indexes.Empty();
//			bNoArrayToHash = false;
//			if (RemovedIndexes.Num() > 0)
//			{
//				RemovedIndexes.Sort();
//				for (int32 i = RemovedIndexes.Num() - 1; i >= 0; --i)
//					ArrayRemoveAt(RemovedIndexes[i]);
//				MarkArrayDirty();
//				RemovedIndexes.Empty();
//				bRealRemoval = true;
//			}
//		}
//		else
//		{
//			bRealRemoval = false;
//			PopSeveralOp();
//		}
//	}
//	else if (DeltaParms.Reader)
//		(*DeltaParms.Reader) << Indexes << RemovedIndexes << bRealRemoval << bNoArrayToHash;
//
//	return bRet;
//}