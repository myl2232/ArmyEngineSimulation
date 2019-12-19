
#include "GTEngineLibrary.h"
#include "EngineGlobals.h"
#include "PublicExportToUE4/GTForUE4Library.h"
#include "PublicExportToUE4/Math/Vector2f.h"
#include "TriangulateCacheData.h"


namespace _InteralTools
{

	static bool ConvertUE4Vector3ArrayToGTEngineVector2Array(const TArray<FVector> & InVertexArray, GTForUE4::Vector2f* OutArray)
	{
		for (size_t Index = 0; Index < InVertexArray.Num(); ++Index)
		{
			OutArray[Index] = GTForUE4::Vector2f(InVertexArray[Index].X, InVertexArray[Index].Y);
		}
		return true;
	}

	static bool ConvertUE4Vector3DoubleArrayToGTEngineVector2Array(const TArray<TArray<FVector>> & InVertexArray, GTForUE4::Vector2f* OutArray)
	{
		size_t Index = -1;
		for(const auto Ref : InVertexArray)
		{
			for (const auto & InRef : Ref)
			{
				OutArray[++Index] = GTForUE4::Vector2f(InRef.X, InRef.Y);
			}			
		}
		return true;
	}

	static bool ConvertGTEngineVector2ArrayToGTEngineVector3Array(const GTForUE4::Vector2f* InArray,const size_t ArraySize,
		const float InReserveZValue,TArray<FVector> & OutVertexArray)
	{
		OutVertexArray.Empty();
		OutVertexArray.Reserve(ArraySize);
		for (size_t Index = 0; Index < ArraySize; ++Index)
		{
			float x = InArray[Index].x;
			float y = InArray[Index].y;
			OutVertexArray.Push(FVector(x, y, InReserveZValue));
		}
		return true;
	}	

	static bool SwapVertexIndex(const uint16_t* InVertexIndexArray,const size_t InVertexIndexCount,const size_t InVertexCount, TArray<uint16> & OutVertexIndexArray)
	{
		OutVertexIndexArray.Empty();
		OutVertexIndexArray.Reserve(InVertexIndexCount);
		for (int32 Index = 0; Index < InVertexIndexCount; ++Index)
		{
			uint16 TrueIndex = InVertexIndexArray[Index];
			if (TrueIndex != 0)
			{
				TrueIndex = InVertexCount - TrueIndex;
			}
			OutVertexIndexArray.Push(TrueIndex);
		}		
		return true;
	}
	static bool SwapVertexIndexForUnColockWise(const uint16_t* InVertexIndexArray, const size_t InVertexIndexCount, const size_t InOuterVertexSize,
		const size_t InVertexCount, TArray<uint16> & OutVertexIndexArray)
	{
		OutVertexIndexArray.Empty();
		OutVertexIndexArray.Reserve(InVertexIndexCount);
		for (int32 Index = 0; Index < InVertexIndexCount; ++Index)
		{
			uint16 TrueIndex = InVertexIndexArray[Index];
			if (TrueIndex != 0 || TrueIndex < InOuterVertexSize)
			{
				TrueIndex = InVertexCount - TrueIndex;
			}
			OutVertexIndexArray.Push(TrueIndex);
		}
		return true;
	}
	static bool GEIndexArrayToUEIndexArray(const uint16_t* InVertexIndexArray, const size_t InVertexIndexCount,  TArray<uint16> & OutVertexIndexArray)
	{
		OutVertexIndexArray.Empty();
		OutVertexIndexArray.Reserve(InVertexIndexCount);
		for (int32 Index = 0; Index < InVertexIndexCount; ++Index)
		{
			uint16 TrueIndex = InVertexIndexArray[Index];			
			OutVertexIndexArray.Push(TrueIndex);
		}
		return true;
	}
	static bool ClockWiseIndicesToUnClockWise(const uint16_t* InVertexIndexArray, const size_t InVertexIndexCount, uint16_t* OutVertexIndexArray)
	{
		check(InVertexIndexCount % 3 == 0);
		for (size_t i = 0; i < InVertexIndexCount; i = i + 3)
		{
			OutVertexIndexArray[i] = InVertexIndexArray[i + 1];
			OutVertexIndexArray[i + 1] = InVertexIndexArray[i];
			OutVertexIndexArray[i + 2] = InVertexIndexArray[i + 2];
		}
		return true;
	}
	template<typename T>
	struct ArrayPtrGuard
	{
	public:
		ArrayPtrGuard(const int32 InSize)			
		{
			Ptr = new T[InSize];
		}
		~ArrayPtrGuard()
		{
			delete[] Ptr;
		}
		T* GetPtr() { return Ptr; }
	private:
		T * Ptr = nullptr;
	};

}

bool UGTEngineLibrary::TriangulatePoly(const TArray<FVector> & InOuterVertexArray, const TArray<FVector> & InInnerVertexArray,
	TArray<FVector> & OutVertexArray, TArray<uint16> & OutVertexIndexArray)
{
	bool Result = true;
	do
	{

		size_t InOuterVertexArraySize = InOuterVertexArray.Num();
		size_t InInnerVertexArraySize = InInnerVertexArray.Num();
		if (InOuterVertexArraySize < 3 || InInnerVertexArraySize < 3)
		{
			return false;
		}
		float reserveZValue = InOuterVertexArray[0].Z;
		TArray<FVector> TempOuterClockWiseVertexArray = InOuterVertexArray;
		TArray<FVector> TempInnerClockWiseVertexArray = InInnerVertexArray;
		_InteralTools::ArrayPtrGuard<GTForUE4::Vector2f> TempInOuterVetexArray(InOuterVertexArraySize);		
		_InteralTools::ArrayPtrGuard<GTForUE4::Vector2f>  TempInInnerVetexArray(InInnerVertexArraySize);		
		_InteralTools::ArrayPtrGuard<GTForUE4::Vector2f>  TempVetexArray(InOuterVertexArraySize + InInnerVertexArraySize);		
		_InteralTools::ArrayPtrGuard<uint16_t>  TempIndexVetexArray((InOuterVertexArraySize + InInnerVertexArraySize) * 6);
		Result = _InteralTools::ConvertUE4Vector3ArrayToGTEngineVector2Array(TempOuterClockWiseVertexArray, TempInOuterVetexArray.GetPtr());
		if (Result == false) break;
		Result = _InteralTools::ConvertUE4Vector3ArrayToGTEngineVector2Array(TempInnerClockWiseVertexArray, TempInInnerVetexArray.GetPtr());
		if (Result == false) break;
		size_t OutIndexSize = 0;
		size_t OutVertexSize = 0;
		Result = GTForUE4::UGTEngineLibrary::TriangulatePoly(TempInOuterVetexArray.GetPtr(), InOuterVertexArraySize,
			TempInInnerVetexArray.GetPtr(), InInnerVertexArraySize, TempVetexArray.GetPtr(), &OutVertexSize, TempIndexVetexArray.GetPtr(), &OutIndexSize);
		if (Result == false) break;
		Result = _InteralTools::ConvertGTEngineVector2ArrayToGTEngineVector3Array(TempVetexArray.GetPtr(), OutVertexSize, reserveZValue, OutVertexArray);
		if (Result == false) break;
		Result = _InteralTools::GEIndexArrayToUEIndexArray(TempIndexVetexArray.GetPtr(), OutIndexSize, OutVertexIndexArray);
		if (Result == false) break;

	} while (false);
	return Result;
}

bool UGTEngineLibrary::TriangulatePoly(const TArray<FVector> & InVertexArray, TArray<uint16> & OutVertexIndexArray)
{
	bool Result = true;
	do
	{

		size_t InVertexArraySize = InVertexArray.Num();
		if (InVertexArraySize < 3)
		{
			return false;
		}
		TArray<FVector> TempClockWiseVertexArray = InVertexArray;
		//TempClockWiseVertexArray.Swap(1, InVertexArraySize - 1);		
		_InteralTools::ArrayPtrGuard<GTForUE4::Vector2f> TempInVetexArray(InVertexArraySize);
		// FixeMe: 事先并无法确切的知道返回数组的大小，所以只能先尽可能的把尺寸放大，GTForUE4 应该提供内存管理方案，避免这种不雅的操作		
		_InteralTools::ArrayPtrGuard<uint16_t> TempZOutIndexVetexArray(InVertexArraySize * 6);
		Result = _InteralTools::ConvertUE4Vector3ArrayToGTEngineVector2Array(TempClockWiseVertexArray, TempInVetexArray.GetPtr());
		if (Result == false) break;
		size_t OutIndexSize = 0;
		Result = GTForUE4::UGTEngineLibrary::TriangulatePoly(TempInVetexArray.GetPtr(), InVertexArraySize, TempZOutIndexVetexArray.GetPtr(), &OutIndexSize);
		if (Result == false) break;
		//Result = _InteralTools::SwapVertexIndex(TempZOutIndexVetexArray, OutIndexSize, InVertexArraySize,OutVertexIndexArray);
		Result = _InteralTools::GEIndexArrayToUEIndexArray(TempZOutIndexVetexArray.GetPtr(), OutIndexSize, OutVertexIndexArray);
		if (Result == false) break;

	} while (false);
	return Result;
}

bool UGTEngineLibrary::TriangulatePolyWithDataCache(const FString & UUID, const TArray<FVector>& InOuterVertexArray, const TArray<FVector>& InInnerVertexArray, TArray<FVector>& OutVertexArray, TArray<uint16>& OutVertexIndexArray)
{
	bool Result = false;
	Result = TriangulatePoly(InOuterVertexArray, InInnerVertexArray, OutVertexArray, OutVertexIndexArray);
	if (Result)
	{
		FTriangulateCacheData::GetInstance().CreateOrUpdateCacheData(UUID, OutVertexArray, OutVertexIndexArray);
	}
	return false;
}

bool UGTEngineLibrary::TriangulatePolyWithDataCache(const FString & UUID, const TArray<FVector> & InVertexArray, TArray<uint16> & OutVertexIndexArray)
{
	bool Result = false;
	Result = TriangulatePoly(InVertexArray, OutVertexIndexArray);
	if (Result)
	{
		FTriangulateCacheData::GetInstance().CreateOrUpdateCacheData(UUID, OutVertexIndexArray);
	}
	return false;
}

bool UGTEngineLibrary::GetCacheData(const FString & InUUID, TArray<uint16>& OutIndexArray)
{
	return FTriangulateCacheData::GetInstance().GetCacheData(InUUID, OutIndexArray);
}

bool UGTEngineLibrary::GetCacheData(const FString & InUUID, TArray<FVector>& OutVertexArray, TArray<uint16>& OutIndexArray)
{
	return FTriangulateCacheData::GetInstance().GetCacheData(InUUID, OutVertexArray, OutIndexArray);
}

bool UGTEngineLibrary::TriangulatePolyWithFastEarcut(const TArray<FVector> & InOuterlineVertexArray, const TArray<TArray<FVector>> & InHoles,
	TArray<FVector> & OutVertices, TArray<uint16> & OutVertexIndexArray,const bool InRetIndeciesClockWise /* = true */)
{
	bool Result = true;
	do
	{
		size_t InOutlineVertexArraySize = InOuterlineVertexArray.Num();
		check(InOutlineVertexArraySize >= 3);
		float reserveZValue = InOuterlineVertexArray[0].Z;
		size_t TotalHolesVertexSize = 0;
		for (const auto & Ref : InHoles)
		{
			TotalHolesVertexSize += Ref.Num();
		}
		TSharedPtr<size_t> TempHolesCountArray = MakeShareable(new size_t[InHoles.Num()]);
		size_t * TempPtr = TempHolesCountArray.Get();
		for (const auto & Ref : InHoles)
		{
			*TempPtr++ = Ref.Num();
		}		
		_InteralTools::ArrayPtrGuard<GTForUE4::Vector2f> TempInOutlineVetexArray(InOutlineVertexArraySize);		
		_InteralTools::ArrayPtrGuard<GTForUE4::Vector2f> TempInInnerHolesVertexArray(TotalHolesVertexSize);		
		_InteralTools::ArrayPtrGuard<GTForUE4::Vector2f> TempOutVerticesArray(InOutlineVertexArraySize + TotalHolesVertexSize);
		_InteralTools::ArrayPtrGuard<uint16_t> TempZOutIndexVetexArray((InOutlineVertexArraySize + TotalHolesVertexSize) * 6);
		Result = _InteralTools::ConvertUE4Vector3ArrayToGTEngineVector2Array(InOuterlineVertexArray, TempInOutlineVetexArray.GetPtr());
		if (Result == false) break;
		Result = _InteralTools::ConvertUE4Vector3DoubleArrayToGTEngineVector2Array(InHoles, TempInInnerHolesVertexArray.GetPtr());
		if (Result == false) break;

		size_t OutIndexSize = 0, OutVertexSize = 0;		
		Result = GTForUE4::UGTEngineLibrary::TriangulatePolyWithFastEarCut(TempInOutlineVetexArray.GetPtr(), InOutlineVertexArraySize,
			TempInInnerHolesVertexArray.GetPtr(),TempHolesCountArray.Get(), InHoles.Num(),
			TempOutVerticesArray.GetPtr(), &OutVertexSize,TempZOutIndexVetexArray.GetPtr() ,&OutIndexSize);	
		if (Result == false) break;

		if (!InRetIndeciesClockWise)
		{			
			_InteralTools::ArrayPtrGuard<uint16_t> TempUnClockWiseIndexArray(OutIndexSize);
			Result = _InteralTools::ClockWiseIndicesToUnClockWise(TempZOutIndexVetexArray.GetPtr(), OutIndexSize, TempUnClockWiseIndexArray.GetPtr());
			if (Result == false) break;
			Result = _InteralTools::GEIndexArrayToUEIndexArray(TempUnClockWiseIndexArray.GetPtr(), OutIndexSize, OutVertexIndexArray);
			if (Result == false) break;
		}
		else
		{
			Result = _InteralTools::GEIndexArrayToUEIndexArray(TempZOutIndexVetexArray.GetPtr(), OutIndexSize, OutVertexIndexArray);
			if (Result == false) break;
		}
			
		Result = _InteralTools::ConvertGTEngineVector2ArrayToGTEngineVector3Array(TempOutVerticesArray.GetPtr(), OutVertexSize, reserveZValue, OutVertices);
		if (Result == false) break;

	} while (false);
	return Result;
}

bool UGTEngineLibrary::CalPloygonDiffenceSet(const TArray<FVector>& InFirstVertexArray, const TArray<FVector>& InSecondVertexArray, TArray<FVector>& OutVertexArray, const bool InRetClockWise)
{
	bool Result = true;
	do
	{
		const size_t InFirstVertexArrayNum = InFirstVertexArray.Num();
		check(InFirstVertexArrayNum >= 3);
		float reserveZValue = InFirstVertexArray[0].Z;	
		const size_t InSecondVertexArrayNum = InSecondVertexArray.Num();
		check(InSecondVertexArrayNum >= 3);		
		_InteralTools::ArrayPtrGuard<GTForUE4::Vector2f> TempFirstVetexArray(InFirstVertexArrayNum);		
		_InteralTools::ArrayPtrGuard<GTForUE4::Vector2f> TempSecondVetexArray(InSecondVertexArrayNum);		
		_InteralTools::ArrayPtrGuard<GTForUE4::Vector2f> TempRetVetexArray(InFirstVertexArrayNum + InSecondVertexArrayNum);

		Result = _InteralTools::ConvertUE4Vector3ArrayToGTEngineVector2Array(InFirstVertexArray, TempFirstVetexArray.GetPtr());
		if (Result == false) break;
		Result = _InteralTools::ConvertUE4Vector3ArrayToGTEngineVector2Array(InSecondVertexArray, TempSecondVetexArray.GetPtr());
		if (Result == false) break;

		size_t RetVertexArrayNum = 0;
		const int PrecisionFactor = 10;			
		Result = GTForUE4::UGTEngineLibrary::CalPloygonDiffenceSet(TempFirstVetexArray.GetPtr(), InFirstVertexArrayNum,
			TempSecondVetexArray.GetPtr(), InSecondVertexArrayNum, PrecisionFactor,
			TempRetVetexArray.GetPtr(), &RetVertexArrayNum, InRetClockWise);
		if (Result == false) break;

		Result = _InteralTools::ConvertGTEngineVector2ArrayToGTEngineVector3Array(TempRetVetexArray.GetPtr(), RetVertexArrayNum, reserveZValue, OutVertexArray);
		if (Result == false) break;

	} while (false);
	return Result;	
}
