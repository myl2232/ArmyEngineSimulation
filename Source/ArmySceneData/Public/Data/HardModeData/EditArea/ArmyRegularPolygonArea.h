#pragma once
#include "DynamicMeshBuilder.h"
#include "ArmyBaseArea.h"
#include "ArmyHelpPolygonArea.h"
enum RegularPolyType
{
	InneraCircle,
	OutCircle
};
class ARMYSCENEDATA_API FArmyRegularPolygonArea :public FArmyBaseArea
{
public:
	FArmyRegularPolygonArea();

	void SetTempArea()
	{
		IsTempArea=true;
	}
	
	void SetRegularPolyType(RegularPolyType InType);
	RegularPolyType GetRegularPolyType()const { return M_RegularPolyType; }
	void SetPosition(FVector InPosition);
	FVector GetPosition()const { return Position; }

	void Move(const FVector& Offset);

	void SetRadius(float InRadius);
	float GetRadius()const { return Radius; }

	void SetBodaThickness(float InThickness);
	float GetBodaThickness()const { return BodaThickness; }
	void SetNumSide(int32 NumSide,bool FreshVertice=true);
	int32 GetNumSide()const { return NumSide; }
	void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)override;
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;

	virtual TSharedPtr<FArmyBaseArea> ForceSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist)override;

	virtual void GenerateLampSlot(
        UWorld* InWorld,
        TSharedPtr<FContentItemSpace::FContentItem> ContentItem,
        float InLampSlotWidth,
        bool bIsLoad = false) override;

	void ApplyTransform(const FTransform& Trans) override;

	void SetExtrusionHeight(float InHeight)override;

	void RefreshExtrusionPlane()override;

	const FBox GetBounds()override;

	void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false)override;

	TArray<FVector> GetOutVertices()const;

	void SetBodaTexture(UTexture2D* InTexture, int32 brickID);
	void SetBodaMaterial(UMaterialInstanceDynamic* InTexture, int32 brickID);

	FVector GetStartPoint()const { return OutStartPoint; }

	void SetStartPoint(FVector InPosition);

	void SetStartPointAndCenterPoint(const FVector& InStartPos, const FVector& InCenterPos);

	//@郭子阳
	void GetConstructionAreas(TArray<FArmyGoods>& ArtificialData)override;

	int32 GetMainBrickID()const {return MainBrickID;};

	virtual void Destroy()override;

	virtual void SetPropertyFlag(PropertyType InType, bool InUse);
	
	virtual EDrawAreaType GetDrawAreaType() override { return EDrawAreaType::RegularPolygon; };
	//@郭子阳 
	//@RetVal 是否是绘制区域
	virtual bool IsPlotArea()  override { return true; }
	// zhx Get InnearHoleSurface For AutoDesign
	TArray<TSharedPtr<FArmyHelpRectArea>> GetInnearHoleSurface() { return InnearHoleSurface; }


protected:
    /** @欧石楠 根据灯槽位置重新计算区域位置和大小 */
    void RecalculateArea(float InLampSlotWidth);

protected:
	//bool IsTempArea = false;

	void CalculateVerticalHole(float deltaHeight, float InLampSlotWidth);
	TSharedPtr<FArmyHelpRectArea> CalculateInnerRectArea(FVector InStart, FVector InEnd, float InDepth);
	void CalculatePlaneInfo(const FVector& InStart, const FVector& InEnd, const float InDepth, FVector& OutPlaneXDir, FVector& OutPlaneYDir, FVector& OutPlaneCenter, TArray<FVector>& OutVertices);

	TArray<TSharedPtr<FArmyHelpRectArea>> InnearHoleSurface;

	virtual void RefreshPolyVertices() override;
	RegularPolyType M_RegularPolyType;
	TArray< FDynamicMeshVertex> BodaAreaVertices;
	TArray<FVector> BodaAreaPositionVertices;
	FVector OutStartPoint;
	FVector Position;
	float Radius;
	float BodaThickness;
	int32 NumSide;
	UMaterialInstanceDynamic* MI_BodaTextureMat;
	int32 MainBrickID;
};

REGISTERCLASS(FArmyRegularPolygonArea)