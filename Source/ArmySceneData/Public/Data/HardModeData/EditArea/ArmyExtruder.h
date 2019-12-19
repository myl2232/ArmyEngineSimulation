/**
 * Copyright 2019 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRExtruder.h
 * @Description 分段放样类物体生成器
 *
 * @Author 欧石楠
 * @Date 2019年5月23日
 * @Version 1.0
 */

#pragma once

#include "ArmyCommonTypes.h"
#include "ArmyActorConstant.h"

using namespace FContentItemSpace;

 /** @欧石楠 放样路径 */
struct FArmyExtrusionPath
{
    FArmyExtrusionPath() { ID = FGuid::NewGuid(); }

    FArmyExtrusionPath(TArray<FVector> InVertexes, bool InbClosed, TSharedPtr<FContentItem> InContentItem)
        : Vertexes(InVertexes)
        , bClosed(InbClosed)
        , ContentItem(InContentItem)
    {
		ID = FGuid::NewGuid();
	}

    FArmyExtrusionPath(TArray<FVector> InVertexes, bool InbClosed, TSharedPtr<FContentItem> InContentItem, int32 InIndex)
        : Vertexes(InVertexes)
        , bClosed(InbClosed)
        , ContentItem(InContentItem)
        , Index(InIndex)
    {
		ID = FGuid::NewGuid();
	}

    void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
    void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

    /** @欧石楠 放样路径顶点数据 */
    TArray<FVector> Vertexes;

    /** @欧石楠 放样路径是否闭合 */
    bool bClosed;

	FGuid ID;

    TSharedPtr<FContentItem> ContentItem;

    int32 Index = 0;
};

class FArmyExtruder
{
public:
    FArmyExtruder(UWorld* InWorld, TSharedPtr<class FArmyBaseArea> InSurfaceArea, TSharedPtr<class FArmyRoomSpaceArea> InRoomSpaceArea)
        : World(InWorld)
        , SurfaceArea(InSurfaceArea)
        , RoomSpaceArea(InRoomSpaceArea)
    {}

    virtual ~FArmyExtruder() {}

    virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
    virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

    /** @欧石楠 生成放样物体 */
    virtual void Generate();

    /** @欧石楠 清空 */
    virtual void Clear();

    /** @欧石楠 将放样路径打断 */
    void SplitPaths(TArray<FArmyExtrusionPath>& InPaths, TArray<FArmyExtrusionPath>& OutPaths, bool bOneSegment = false, bool bLeft = true);

    /** @欧石楠 删除一段放样物体 */
    virtual void DeleteActor(class AArmyExtrusionActor* InActor);
    virtual TSharedPtr<FContentItem> DeleteActorByVertex(const FVector& InVertex);

    bool IsEmpty() { return Paths.Num() <= 0; }

    FString GetDXFFilePath(TSharedPtr<FContentItem> InContentItem);
    const int32 GetSaleID() const;
    TArray<FArmyExtrusionPath>& GetPaths() { return Paths; }
    TMap<class AArmyExtrusionActor*, FArmyExtrusionPath> GetActorMap() { return ActorMap; }

    /** @欧石楠 获取施工算量周长 */
    float GetCircumference();

    FContentItemPtr GetContentItem();

    /** @欧石楠 偏移放样模型的位置 */
    ARMYSCENEDATA_API virtual void ApplyOffset(const FVector& InOffset);

    void SetPaths(TArray<FArmyExtrusionPath> InPaths) { Paths = InPaths; }

    /** @欧石楠 设置离地高度 */
    void SetDistanceToFloor(const float InDistanceToFloor);

protected:
    virtual FString GetUniqueCode() { return FString(); }
    virtual FName GetFolderPath() { return FName(); }

    /** @欧石楠 是否需要翻转上下位置 */
    virtual bool NeedReverse() { return false; }

    virtual AArmyExtrusionActor* GenerateActor(const FArmyExtrusionPath& Path);
    virtual AArmyExtrusionActor* GenerateRelatedActor(FArmyExtrusionPath& Path, bool bLeft);

protected:
    UWorld* World;

    TSharedPtr<FArmyBaseArea> SurfaceArea;

    TSharedPtr<FArmyRoomSpaceArea> RoomSpaceArea;

    TMap<AArmyExtrusionActor*, FArmyExtrusionPath> ActorMap;

    TArray<FArmyExtrusionPath> Paths;
};