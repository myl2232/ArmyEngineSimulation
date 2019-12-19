#pragma once

#include "ArmyExtruder.h"

/** @欧石楠 灯带路径 */
struct FArmyLightPath
{
    FArmyLightPath() {}

    FArmyLightPath(FVector InStartPos, FVector InEndPos)
        : StartPos(InStartPos)
        , EndPos(InEndPos)
    {}

    void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
    void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

    FVector StartPos;
    FVector EndPos;
};

class ARMYSCENEDATA_API FArmyLampSlotExtruder : public FArmyExtruder
{
public:
	FArmyLampSlotExtruder(UWorld* InWorld, TSharedPtr<class FArmyBaseArea> InSurfaceArea);

    virtual void DeleteActor(class AArmyExtrusionActor* InActor) override;
    virtual void Generate() override;
    virtual void Clear() override;
    virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
    virtual void ApplyOffset(const FVector& InOffset) override;

    TArray<FGuid>& GetLampLightIDs() { return LampLightIDs; }
    const TArray<FVector>& GetAreaVertexes() { return AreaVertexes; }
    const TSparseArray<FArmyLightPath>& GetLightConstructionInfo() { return LightConstructionInfo; }

protected:
    virtual FName GetFolderPath();
    virtual AArmyExtrusionActor* GenerateActor(const FArmyExtrusionPath& Path) override;
    virtual AArmyExtrusionActor* GenerateRelatedActor(FArmyExtrusionPath& Path, bool bLeft) override;
    virtual TSharedPtr<FContentItem> DeleteActorByVertex(const FVector& InVertex) override;

    class APointLight* GenerateLight(FVector StartPos, FVector EndPos, const int32 Index);
    AArmyExtrusionActor* GenerateLightModel(FVector StartPos, FVector EndPos, const int32 Index);
    void RecaculateLightModelPostion(AArmyExtrusionActor* LightModel, FVector StartPos, FVector EndPos, const int32 Index);

    /** @欧石楠 单段灯槽删除后需要重新计算吊顶 */
    void UpdateSuspendedCeiling(const int32& InIndex);

    void RemoveLightModelByActor(AArmyExtrusionActor* InActor);

    TArray<FVector> ExtrusionVertexes(const TArray<FVector>& InVertexes, const float InExtrusionDistance);

protected:
    // @打扮家 XRLightmass 序列化 与LampLights等位存储GUID
    TArray<FGuid> LampLightIDs;

    TMap<AArmyExtrusionActor*, AArmyExtrusionActor*> LightMap;

    TArray<FVector> AreaVertexes;

    // @欧石楠 灯带施工图顶点数据
    TSparseArray<FArmyLightPath> LightConstructionInfo;

public:
	//@郭子阳
	//所有的灯带

	static  TMap< FGuid, AArmyExtrusionActor* >& GetAllLightModel();
};