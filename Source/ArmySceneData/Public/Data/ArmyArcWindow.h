#pragma once

#include "ArmyObject.h"
#include "ArmyEditPoint.h"

class ARMYSCENEDATA_API FArmyArcWindow :public FArmyObject
{
public:
	FArmyArcWindow();
	FArmyArcWindow(FArmyArcWindow* Copy);
	virtual ~FArmyArcWindow() {}

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	//~ Begin FArmyObject Interface
	virtual void SetState(EObjectState InState) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual const FBox GetBounds() override;
	//~ End FArmyObject Interface

	virtual TSharedPtr<FArmyEditPoint> SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual TSharedPtr<FArmyEditPoint> HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual void GenerateWindowModel(UWorld* World);
	virtual EObjectType GetType() const override;
	virtual void Generate(UWorld* InWorld) override;
	void DestroyWindowModel();

	/** bsp生成裁剪 */
	TArray<FVector> GetClipingBox();
	/** 设置起点*/
	void SetArcWindowStartPos(const FVector& InStartPos);
	/**设置终点*/
	void SetArcWindowEndPos(const FVector& InEndPos);
	/**设置起点和终点和中心点*/
	void SetArcWindowStartAndEndPos(const FVector& InStartPos, const FVector& InEndPos, const FVector& InCenterPos);
	/**设置终点和中心点*/
	void SetArcWindowEndAndCenterPos(const FVector& InEndpos, const FVector& InCenterPosition);
	/**设置弧形窗的厚度*/
	void SetArcWindowThickness(float InThickness);
	/**获取弧形窗厚度*/
	float GetArcWindowThickness() { return ArcWindowThickness; }
	/** 设置弧形窗属性*/
	void SetProperty(FVector StartPos, FVector EndPos, FVector MiddlePos, float Thickness);
	/**设置弧形窗的高度*/
	float GetWindowHeight() const { return WindowHeight; }
	/**设置弧形窗高度*/
	void SetWindowHeight(float InHeight);
	/** 设置弧形窗的离地高度*/
	void SetWindowOffGroundHeight(float InOffsetGround);
	/** 获取弧形窗离地高度*/
	float GetWindowOffGroundHeight()const { return HeightToFloor; }
public:
	TSharedPtr<FArmyEditPoint> LeftStaticPoint;
	TSharedPtr<FArmyEditPoint> RightStaticPoint;
	TSharedPtr<FArmyEditPoint> ArcMiddlePoint;
protected:

	void UpdateWindow();

	void UpdateCirclePos(TSharedPtr<class FArmyArcLine> InCircle, float offset);
	class AXRShapeActor* WindowSill;
	class AXRShapeActor* WindowGlass;
	class AXRShapeActor* WindowPillar;
	class AXRShapeActor* WindowWall;

	void GenerateWindowSill();
	void GenerateWindowGlass();
	void GenerateWindowPillar();
	void GenerateWindowWall();
	TArray<FVector> CaculatePolygonVertices(float InThickNess, float InHeight);
	void DeselectPoints();
	
	TSharedPtr<class FArmyArcLine> OutArcLine;
	TSharedPtr<class FArmyArcLine> InnearArcLine;
	TSharedPtr<class FArmyArcLine> MiddleUpArcLine;
	TSharedPtr<class FArmyArcLine> MiddleDownArcLine;
	TSharedPtr<class FArmyLine> LeftLine;
	TSharedPtr<class FArmyLine> RightLine;
	FVector ArcWindowStartPos;
	FVector ArcWindowEndPos;
	FVector ArcWindowMiddlePos;
	float WindowHeight;
	float HeightToFloor;
	FVector ArcWindowCenterPos;
	float ArcWindowAngle;
	float ArcWindowThickness;

};
REGISTERCLASS(FArmyArcWindow)