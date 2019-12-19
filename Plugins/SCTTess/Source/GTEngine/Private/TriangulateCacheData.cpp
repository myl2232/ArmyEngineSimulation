#include "TriangulateCacheData.h"

void FTriangulateCacheData::CreateOrUpdateCacheData(const FString & InUUID, const TArray<uint16>& InIndexArray)
{
	auto Iter = CacheData.Find(InUUID);
	if (Iter)
	{
		StaticCast<FTriangulateCacheData::FCacheDataWithoutHole*>(Iter->Get())->IndexArray = InIndexArray;
	}
	else
	{
		TSharedPtr<FTriangulateCacheData::FCacheDataWithoutHole>  RefData = MakeShareable(new FTriangulateCacheData::FCacheDataWithoutHole(InUUID));
		RefData->IndexArray = InIndexArray;
		CacheData.Add(InUUID, RefData);		
	}
}

void FTriangulateCacheData::CreateOrUpdateCacheData(const FString & InUUID, const TArray<FVector>& InVertexArray, const TArray<uint16> InIndexArray)
{

	auto Iter = CacheData.Find(InUUID);
	if (Iter)
	{
		StaticCast<FTriangulateCacheData::FCacheDataWihtHole*>(Iter->Get())->IndexArray = InIndexArray;
		StaticCast<FTriangulateCacheData::FCacheDataWihtHole*>(Iter->Get())->VertexArray = InVertexArray;
	}
	else
	{
		TSharedPtr<FTriangulateCacheData::FCacheDataWihtHole>  RefData = MakeShareable(new FTriangulateCacheData::FCacheDataWihtHole(InUUID));
		RefData->IndexArray = InIndexArray;
		RefData->VertexArray = InVertexArray;
		CacheData.Add(InUUID, RefData);
	}

}

bool FTriangulateCacheData::GetCacheData(const FString & InUUID, TArray<uint16>& OutIndexArray) const
{
	bool Result = false;
	auto Iter = CacheData.Find(InUUID);
	if (Iter)
	{
		OutIndexArray = StaticCast<FTriangulateCacheData::FCacheDataWithoutHole*>(Iter->Get())->IndexArray;
		Result = true;
	}
	return Result;
}

bool FTriangulateCacheData::GetCacheData(const FString & InUUID, TArray<FVector>& OutVertexArray, TArray<uint16>& OutIndexArray) const
{
	bool Result = false;
	auto Iter = CacheData.Find(InUUID);
	if (Iter)
	{
		OutIndexArray = StaticCast<FTriangulateCacheData::FCacheDataWihtHole*>(Iter->Get())->IndexArray;
		OutVertexArray = StaticCast<FTriangulateCacheData::FCacheDataWihtHole*>(Iter->Get())->VertexArray;
		Result = true;
	}
	return Result;
}

FTriangulateCacheData & FTriangulateCacheData::GetInstance()
{
	check(Instance);
	return *Instance;
}

bool FTriangulateCacheData::StartUp()
{
	check(!Instance);	
	Instance = new FTriangulateCacheData();
	return true;
}

bool FTriangulateCacheData::ShutDown()
{
	check(Instance);
	delete Instance;
	Instance = nullptr;
	return true;
}

FTriangulateCacheData * FTriangulateCacheData::Instance = nullptr;