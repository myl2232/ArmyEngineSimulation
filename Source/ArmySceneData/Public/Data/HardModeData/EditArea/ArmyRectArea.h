#pragma once

#include "ArmyBaseArea.h"
#include "ArmyHelpPolygonArea.h"
#include "DynamicMeshBuilder.h"
#include "ArmyBodaArea.h"
#include "ArmyCircleArea.h"

class ARMYSCENEDATA_API FArmyRectArea :public FArmyBaseArea
{
public:
	FArmyRectArea();

	virtual  void IntervalDistChange(float InV)override;

	void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)override;

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual	bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist)override;
	virtual TSharedPtr<FArmyBaseArea> ForceSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist)override;

	virtual bool HoverSingleEdge(const FVector2D& Pos, class UArmyEditorViewportClient* ViewPortClient, FVector& start, FVector& end);

	void ApplyTransform(const FTransform& Trans) override;

	void SetVertice(const TArray<FVector>& InVertices);

	void SetStyle(TSharedPtr<class FArmyBaseEditStyle> InStyle) override;

	void PostModify(bool bTransaction/* =false */)override;

	const FBox GetBounds()override;

	void SetRectWidth(const float InWidth);

	float GetRectWidth()const { return AreaWidth; }

	void SetRectHeight(const float InHeight);

	float GetRectHeight()const { return AreaHeight; }

	void SetLeftUpCornerPos(const FVector InCenterPosition);

	FVector GetLeftUpCornerPos()const { return  LeftUpCornerPoint; }

	void SetCenterPosition(const FVector InPosition);

	FVector GetCenterPosition();

	const TArray<FVector>& GetOutVertices()const;

	void SetCornerType(BodaCornerType InType);
	BodaCornerType GetCornerType() const { return M_DodaType; }

	/* @梁晓菲 区域修改的时候更新波打线*/
	void RefreshBodaBrick();

	void SetBodaBrickAndThickness(float length, float width, TSharedPtr<FContentItemSpace::FContentItem > ContentItem);
	float GetBodaThickness()const { return BodaThickness; }

	void SetBodaMaterial(UMaterialInstanceDynamic* InBodaMaterial, int32 mainID);

	//判断是否有波打线
	bool HasBodaThickness() const;

	void DeleteBodaArea();

	virtual void SetExtrusionHeight(float height)override;

	virtual void RefreshExtrusionPlane()override;

	virtual void GenerateLampSlot(
        UWorld* InWorld,
        TSharedPtr<FContentItemSpace::FContentItem> ContentItem,
        float InLampSlotWidth,
        bool bIsLoad = false) override;

	
	virtual bool TestAreaCanAddOrNot(const TArray<FVector>& InTestArea, FGuid ObjId, TSharedPtr<FArmyBaseArea> OutParent = nullptr);
	
	virtual EDrawAreaType GetDrawAreaType() override { return EDrawAreaType::Rectangle; };
	//导出施工项
	//@郭子阳
	void GetConstructionAreas(TArray<FArmyGoods>& OutAreas)override;

	void DestroyAllLights();

	void GenerateAllLights();

	void RemoveLampSlot(AArmyExtrusionActor* InLampSlotActor) override;

	bool HasLight();

	void Destroy() override;

	void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false)override;

	virtual void SetPropertyFlag(PropertyType InType, bool InUse)override;

	virtual void SetActorVisible(bool InVisible)override;

	int32 GetCornerBrickID()const { return CornerBrickID; }
	int32 GetMainBrickID()const { return MainBrickID; }

	//获取区域的外圈线段
	void GetConstructionOutLines(TArray< TSharedPtr<FArmyLine> >& OutLines);
	//获取内圈线段--->波动线
	void GetConstructionInnerLines(TArray< TSharedPtr<FArmyLine> >& OutLines);

	uint32 DriveByHoleOrBoundary = 0;// 0 表示整体驱动，1 表示单边驱动

	FSimpleDelegate BoundaryDriveEvent;

	void StartTransformByEdge();

	uint32 CurrentTransformFlag = 0;

	virtual uint32 GetTransformFlag() { return  TransformFlag; };
	//@郭子阳 
	//@RetVal 是否是绘制区域
	virtual bool IsPlotArea()  override { return true; }

	//@郭子阳
	//单边移动
	void MoveByEdge(FVector offset);

	//virtual TSharedPtr<FArmyBaseArea> FindArea(const FVector& InPoint) override;

	virtual void Move(const FVector& Offset) override;
    virtual void UpdateWallArea(TArray<FVector> NewVertexes) override;

	// zhx Get InnearHoleSurface For AutoDesign
	TArray<TSharedPtr<FArmyHelpRectArea>> GetInnearHoleSurface() { return InnearHoleSurface;}

protected:
    /** @欧石楠 根据灯槽位置重新计算区域位置和大小 */
    void RecalculateArea(float InLampSlotWidth);

private:
	TSharedPtr<FArmyHelpRectArea> CalculateInnerRectArea(FVector InStart, FVector InEnd, float InDepth);
	
	void CalculatePlaneInfo(const FVector& InStart, const FVector& InEnd, const float InDepth, FVector& OutPlaneXDir, FVector& OutPlaneYDir, FVector& OutPlaneCenter, TArray<FVector>& OutVertices);

	TArray<TSharedPtr<FArmyHelpRectArea>> InnearHoleSurface;
	void ExtrusionBodaArea();
	void RefreshPolyVertices()override;
    void CalculateVerticalHole(float deltaHeight, float lampSlotWidth);
	void CalculateVerticalHole(float deltaHeight, float lampSlotWidth, TArray<FVector> NewVertexes);
	FVector LeftUpCornerPoint;
	float BodaThickness;
	float  BodaBrickLength = 40;
	float AreaWidth;
	float AreaHeight;
	BodaCornerType M_DodaType;
	TSharedPtr<class FArmyRect> OwnerRect;

	int32 CornerBrickID;
	int32 MainBrickID;

	TArray< FDynamicMeshVertex> BodaCornerVertices;
	TArray<FVector> BodaCornerPositionVertices;

	UMaterialInstanceDynamic* MI_BodaTextureMat;
	UMaterialInstanceDynamic* MI_CornerTextureMat;
};

REGISTERCLASS(FArmyRectArea)