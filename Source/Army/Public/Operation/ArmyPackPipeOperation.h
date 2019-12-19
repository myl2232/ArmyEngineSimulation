#pragma once
#include "ArmyOperation.h"

class  FArmyPackPipeOperation :public FArmyOperation
{
public:
	FArmyPackPipeOperation(EModelType InBelongModel);
	virtual ~FArmyPackPipeOperation() {}
	virtual void BeginOperation(XRArgument InArg = XRArgument()) override;
	virtual void EndOperation() override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;	
	virtual void Tick() override;
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;
	virtual TArray< TSharedPtr<class FArmyLine> >& GetLines() override { return CachedLines; }
	virtual void MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y) override;
	void GenerateWalls();
	bool IsOperating();

protected:

	void Exec_AddSpace(TArray<FVector>& Vertexes);

	FVector Exec_PopPoint();
	void Exec_PushPoint(const FVector& Pos);

	bool GenerateExtrutionPoints(TArray<FVector>& InPoints);

	void OnConfirmAddWallClicked();

	void OnCloseAddWallClicked();

	void OnLineInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

private:
	TArray<FVector> CachedPoints;
	/** @欧石楠 缓存下点击时存下的点*/
	TArray<FVector> CachedDrawPoints;

	TArray< TSharedPtr<FArmyLine> > CachedLines;

	TSharedPtr<class FArmyLine> SolidLine;
	TSharedPtr<class FArmyPolygon> Polygon;

	XRArgument CurrentArg;
	bool bDrawPolygon = false;

	bool bInternalExtrusion = true;
	float ExtrusionThickness = 12;//单位厘米
	bool bOffsetGenerate = false;

	TSharedPtr<class SArmyAddWallSetting> SettingWidget;
	TSharedPtr<class FArmyRulerLine> RulerLine;

	TArray<FVector> InsideVertices;
	TArray<FVector> OutsideVertices;
};