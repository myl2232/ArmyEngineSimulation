#pragma once

#include "ArmyObject.h"

class ARMYSCENEDATA_API FArmyArcWall :public FArmyObject
{
public:
	FArmyArcWall();

	FArmyArcWall(FArmyArcWall* Copy);

	virtual	~FArmyArcWall();

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;

	virtual void SetState(EObjectState InState) override;

	virtual void Refresh() override;

	/** 绘制 */
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	/** 选中 */
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	virtual TSharedPtr<FArmyEditPoint> SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual TSharedPtr<FArmyEditPoint> HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;

	/** 高亮 */
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	/**	获取所有线*/
	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false);

	virtual const void GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints)override;

	virtual void GetCircleLines(TArray<TSharedPtr<FArmyCircle>>& OutCircle);

	virtual void GetArcLines(TArray<TSharedPtr<class FArmyArcLine>>& OutArcLines);

	/**	获取包围盒*/
	virtual const FBox GetBounds();

	virtual EObjectType GetType() const override;

	void SetArcWallStart(FVector InStartPoint);

	void SetArcWallEnd(FVector InEndPoint);

	FORCEINLINE const FVector GetStart ()
	{
		return MArcLine->GetStartPos();
	}
	FORCEINLINE const FVector GetEnd ()
	{
		return MArcLine->GetEndPos();
	}
	
	FORCEINLINE const FVector GetCenter ()
	{
		return MArcLine->GetArcMiddlePos();
	}

	void SetMousePos(FVector ArcMiddlePoint);

	void UpdateMousePosition(FVector InMousePosition);

	void UpdateAuxiliaryLine();

	void SetArcWallThreePoint(FVector InStartPos, FVector InEndPos, FVector InMiddlePos);

	void UpdateArcVertices();

	void SetOffsetHeight(float dist);

	float GetOffsetHeight()const;

	float GetWallRadius()const;

	FVector GetCenterPosition()const;

	float GetArcAngle()const;

	bool GetIsUp();

	const TArray<FVector>& GetResultVertices()const;

	virtual void Generate(UWorld* InWorld) override;

public:
	int32 CurrentState;//-1 未开始，1已绘制第一个基点，2已绘制第二个基点，0已完成绘制
	TSharedPtr<FArmyEditPoint> LeftStaticPoint;
	TSharedPtr<FArmyEditPoint> RightStaticPoint;
	TArray<FVector> ResultVertices;
	TSharedPtr<FArmyEditPoint> ArcMiddlePoint;

	bool bShowPoint;
private:
	void DeselectPoints();
	FLinearColor ArcWallColor;
	float OffsetHeight;
	TSharedPtr<FArmyLine> MainLine;
	TSharedPtr<class FArmyArcLine> MArcLine;
	bool IsUp;
	FVector centerPosition;
	float WallRadius;

	float ArcWallAngle;
};
REGISTERCLASS(FArmyArcWall)

