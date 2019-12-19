#pragma once
#include "CoreMinimal.h"

class ARMYTOOLS_API "FArmyAxisRulerActor :
	public TSharedFromThis<"FArmyAxisRulerActor>
{
public:
	struct AxisInfo
	{
		bool Visible = true;
		UWorld* TheWorld = NULL;
		FVector TraceStart = FVector(ForceInitToZero);
		FVector TraceEnd = FVector(ForceInitToZero);
		FColor AxisColor;
	};
	enum AxisType
	{
		AXIS_XFRONT,
		AXIS_XBACK,
		AXIS_YFRONT,
		AXIS_YBACK,
		AXIS_ZFRONT,
		AXIS_ZBACK,
		AXIS_ALL
	};
public:
	"FArmyAxisRulerActor();
	~"FArmyAxisRulerActor();

	void Init(TSharedPtr<SOverlay> ParentWidget);

	void OnViewTypeChanged(int32 InViewType);

	void SetAxisVisible(AxisType InType,bool InVisible);

	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	void Update(class UArmyEditorViewportClient* InViewportClient);
private:
	bool OnCollisionLocation(UWorld* InWorld,const FVector& InTraceStart, const FVector& InTraceEnd, FHitResult& OutHit, const TArray<AActor*>& InIgnoredArray);

	void DrawMeasureLine(UWorld* InWorld,FVector InStart, FVector InEnd, FColor InColor);
	//正则
	bool CheckStringIsVaild(const FString& str);

	void OnTextCommitted(const FText&, const SWidget*);
private:
	TSharedPtr<class SSArmyEditableLabel> XFrontInputBox;
	TSharedPtr<class SSArmyEditableLabel> XBackInputBox;
	TSharedPtr<class SSArmyEditableLabel> YFrontInputBox;
	TSharedPtr<class SSArmyEditableLabel> YBackInputBox;
	TSharedPtr<class SSArmyEditableLabel> ZFrontInputBox;
	TSharedPtr<class SSArmyEditableLabel> ZBackInputBox;

	float XFrontV = 0;
	float XBackV = 0;
	float YFrontV = 0;
	float YBackV = 0;
	float ZFrontV = 0;
	float ZBackV = 0;

	AxisInfo XFrontAxis;
	AxisInfo XBackAxis;
	AxisInfo YFrontAxis;
	AxisInfo YBackAxis;
	AxisInfo ZFrontAxis;
	AxisInfo ZBackAxis;

	bool ShowAxis = false;

	int32 CurrentViewType = 0;
};
