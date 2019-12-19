#pragma once
/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRBearingWallOperation.h
 * @Description 承重墙操作逻辑
 *
 * @Author 
 * @Date 2018年11月5日
 * @Version 1.0
 */
#include "ArmyOperation.h"
#include "ArmyBearingWall.h"

class FArmyBearingWallOperation :public FArmyOperation
{
public:
	FArmyBearingWallOperation(EModelType InBelongModel);
	~FArmyBearingWallOperation();

	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual void BeginOperation(XRArgument InArg = XRArgument()) override;
	virtual void EndOperation() override;
	virtual void Tick() override;
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;
	virtual void MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y) override;
	TSharedPtr<class FArmyBearingWall> Exec_AddBearingWall(TArray<FVector>& Vertexes);

private:
	struct FArmyBearingWall::FDoubleWallCaptureInfo CaptureWallInfo;
	bool CaptureDoubleLine(const FVector& Pos, FArmyBearingWall::FDoubleWallCaptureInfo& OutInfo);

	TArray<TWeakPtr<FArmyLine>> AllLines;
	TArray<TWeakPtr<FArmyLine>> OutWallLines;
	TArray< TSharedPtr<FArmyLine> > CachedLines;
	TSharedPtr<class FArmyLine> SolidLine;

	TArray<TSharedPtr<FArmyPolygon>> MofidyWallRectList;
	bool bShouldCreate = false;
};