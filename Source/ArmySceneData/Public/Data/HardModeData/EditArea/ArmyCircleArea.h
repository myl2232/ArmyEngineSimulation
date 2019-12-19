#pragma once
#include "ArmyBaseArea.h"

class ARMYSCENEDATA_API FArmyCircleArea :public FArmyBaseArea
{
public:
	FArmyCircleArea();
	~FArmyCircleArea();
	/**
	* 设置圆形区域圆心
	*/
	void SetPosition(FVector InPosition);
	FVector GetPosition()const { return Position; }

	virtual void Move(const FVector& Offset) override;

	//获取世界圆心坐标
	FVector GetPosition3D()const;

	void SetRadius(float InRadius);
	float GetRadius()const { return Radius; }

	void SetCenterAndRadius(const FVector& InCenter, float InRadius);

	void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)override;

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;

	virtual TSharedPtr<FArmyBaseArea> ForceSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist)override;

	void ApplyTransform(const FTransform& Trans) override;

	void GetCircleLines(TArray<TSharedPtr<FArmyCircle>>& OutCircle)override;

	const FBox GetBounds();

	void SetExtrusionHeight(float InHeight)override;

	void RefreshExtrusionPlane()override;
   
	void SetPropertyFlag(PropertyType InType, bool InUse)override;

	virtual void GenerateLampSlot(
        UWorld* InWorld,
        TSharedPtr<FContentItemSpace::FContentItem> ContentItem,
        float InLampSlotWidth,
        bool bIsLoad = false) override;

	virtual void Destroy()override;

	virtual EDrawAreaType GetDrawAreaType() override { return EDrawAreaType::Circle; };
	//@郭子阳 
	//@RetVal 是否是绘制区域
	virtual bool IsPlotArea()  override { return true; }
	// zhx Get InnearHoleSurface For AutoDesign
	TSharedPtr<class FArmyClinderArea> GetInnearHoleSurface() { return ClinderArea; }
	
	//@郭子阳
	//设置为临时面
	void SetIsTempArea();
protected:

	bool IsTempArea = false;
	void RefreshPolyVertices()override;

	TSharedPtr<class FArmyClinderArea> ClinderArea = NULL;

	FVector Position;
	float Radius;
	TSharedPtr<class FArmyCircle> MCircle;

};

REGISTERCLASS(FArmyCircleArea)