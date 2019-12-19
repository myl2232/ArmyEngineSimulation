#pragma once
#include "ArmyOperation.h"
#include "ArmyModifyWall.h"

class FArmyModifyWallOperation :public FArmyOperation
{
public:
	FArmyModifyWallOperation(EModelType InBelongModel);
	~FArmyModifyWallOperation();

	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual void BeginOperation(XRArgument InArg = XRArgument()) override;
	virtual void EndOperation() override;
	virtual void Tick() override;
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;
	virtual void MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y) override;
	TSharedPtr<class FArmyModifyWall> Exec_AddModifyWall(TArray<FVector>& Vertexes);

private:
	

private:
	struct FArmyModifyWall::FDoubleWallCaptureInfo CaptureWallInfo;
	bool CaptureDoubleLine(const FVector& Pos, FArmyModifyWall::FDoubleWallCaptureInfo& OutInfo);

	TArray<TWeakPtr<FArmyLine>> AllLines;
	TArray<TWeakPtr<FArmyLine>> OutWallLines;
	TArray< TSharedPtr<FArmyLine> > CachedLines;
	TSharedPtr<class FArmyLine> SolidLine;

	TArray<TSharedPtr<FArmyPolygon>> MofidyWallRectList;
	bool bShouldCreate = false;
};