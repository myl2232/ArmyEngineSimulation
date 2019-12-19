#pragma once
#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"

class FCabinetShape;
class FDoorGroup;
class FSpaceShape;
class FBoardShape;
struct FCommonPakData;
struct FMtlInfo;

namespace XRShapeUtilities
{
    // 返回true，继续递归
    // 返回false，不再向下递归
    DECLARE_DELEGATE_RetVal_OneParam(bool, FOnIterateCabinet, FSpaceShape*)
    // 递归遍历柜子的所有层级
    void IterateCabinet(FCabinetShape *InCabShape, FOnIterateCabinet OnIterateCabinet);
    // 获取柜子中特定的柜体框架板件
    TSharedPtr<FBoardShape> GetCabinetFrameBoard(FCabinetShape *InCabShape, int32 Type);

    #define FLAG_SLIDINGDOORS 0x01
    #define FLAG_SIDEHUNGDOORS 0x02
    #define FLAG_ALLDOORS (FLAG_SLIDINGDOORS | FLAG_SIDEHUNGDOORS)
    // 获取柜子中所有的门板
    void GetCabinetDoors(FCabinetShape *InCabShape, TArray<FDoorGroup*> &OutDoorGroups, int32 Flag = FLAG_ALLDOORS);
    // 获取柜子中所有的抽面
    void GetCabinetDrawerDoors(FCabinetShape *InCabShape, TArray<FDoorGroup*> &OutDrawerDoors);
    // 生成材质对象
    TSharedPtr<FCommonPakData> CreateMtlPakData(const FMtlInfo * InMtlInfo);
    // 加载一个材质
    class UMaterialInterface* LoadMtl(FMtlInfo * InMtlInfo);
}