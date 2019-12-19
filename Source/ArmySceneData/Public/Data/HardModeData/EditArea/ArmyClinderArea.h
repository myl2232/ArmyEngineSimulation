#pragma once
#include "ArmyBaseArea.h"

class ARMYSCENEDATA_API  FArmyClinderArea : public FArmyBaseArea
{
public:
	FArmyClinderArea();
	FArmyClinderArea(FVector InPos, float InRadius, float InHeight);
	~FArmyClinderArea();
	// 设置圆柱体位置，半径，法相信息
	void SetClinderPositionInfo(const FVector& InCenter, const float InRadius, const float InHeight);
	void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)override;
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual	bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist) override;
	void SetStyle(TSharedPtr<class FArmyBaseEditStyle> InStyle)override;

	virtual	void GenerateExtrudeActor(UWorld* InWorld, TSharedPtr<FContentItemSpace::FContentItem> ContentItem) override;

	void GetClinderInfo(FVector& OutCenter, float& OutRadius, float& OutHeight);
private:
	void CaculateClinderWallActor();
	void RefreshPolyVertices();
	FString TextureUrl;
	FVector Position;
	float Radius;

	float M_ClinderHeight;
	float XRepeatDis = 100.0f;
	float YRepeatDis = 100.0f;
	TArray<FDynamicMeshVertex> TotalVertices;
	TArray<FVector> TempTotalVertices;
	UMaterialInstanceDynamic* MI_FloorTextureMat;

};

REGISTERCLASS(FArmyClinderArea)