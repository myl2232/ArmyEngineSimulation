#pragma once
#include "CoreMinimal.h"
#include "ArmyObject.h"

DECLARE_DELEGATE_RetVal_OneParam(bool, FAxisRuler3DDelegate, FVector);

class ARMYTOOLS_API "FArmyAxisRuler3D :
	public TSharedFromThis<"FArmyAxisRuler3D>
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
	"FArmyAxisRuler3D();
	~"FArmyAxisRuler3D();

	void Init(TSharedPtr<SOverlay> ParentWidget);

	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);
	void Draw (const FSceneView* InView,FCanvas* SceneCanvas) ;

	void Update(UArmyEditorViewportClient* InViwportClient,const FSceneView* InView,const TArray<FObjectWeakPtr>& InObjects,FVector Position,FVector InNormal,FCanvas* SceneCanvas,FVector InWallDiection);

	void UpdateInfo(UArmyEditorViewportClient* InViwportClient,const FSceneView* InView,const TArray<FObjectWeakPtr>& InObjects,FVector Position,FVector InNormal,FCanvas* SceneCanvas, FVector InWallDiection);

	void Axis3DCollision(UArmyEditorViewportClient* InViwportClient);

	void Update3D(UArmyEditorViewportClient* InViwportClient,const FSceneView* InView,const TArray<FObjectWeakPtr>& InObjects,FVector Position,FVector InNormal,FCanvas* SceneCanvas);

	void Update2D(UArmyEditorViewportClient* InViwportClient,const FSceneView* InView,const TArray<FObjectWeakPtr>& InObjects,FVector Position,FVector InNormal,FCanvas* SceneCanvas);

	void WorldToPixel(UArmyEditorViewportClient* InViwportClient,const FVector& WorldPoint,FVector2D& OutPixelLocation);

	void GetLenght(TSharedPtr<"FArmyLine> Line,FText& LenText);

	bool OnCollisionLocation(UWorld* InWorld,const FVector& InTraceStart,const FVector& InTraceEnd,FHitResult& OutHit);

	/**@刘克祥控制各个轴标尺的显示*/
	void SetAxisRulerDisplay(bool InShowXAxis, bool InShowYAxis, bool InShowZAxis);

	//判断是否在墙上
	bool IsOnWall(FVector& InMousePoint,FVector& InNormal);

	//判断是否在地板
	bool IsOnFloor(FVector& InMousePoint,FVector& InNormal);

	//判断是否在墙顶
	bool IsOnTop(FVector& InMousePoint,FVector& InNormal);

	//判断是否在地板or墙顶
	bool IsOnFloorOrTop(FVector& InMousePoint,FVector& InNormal);

	void SetActor(AActor* InActor);

	void Show(bool inEnable);

	FBox GetBox();

	void ConversionLocation(class UStaticMeshComponent* InComponent,FVector& InRelativeLocation,FVector& OutLocation);

	void SetRefCoordinateSystem(const FVector& InBasePos,const FVector& InXDir,const FVector& InYDir,const FVector& InNormal);
	const FPlane& GetPlane() const;

	void DrawMeasureLine(UWorld* InWorld,FVector InStart,FVector InEnd,FColor InColor);
private:

	//正则
	bool CheckStringIsVaild (const FString& str);

	void OnTextCommitted (const FText&,const SWidget*);

public:
	FAxisRuler3DDelegate AxisOperationDelegate;
private:

	FVector BasePoint;
	FVector BaseNormal;
	FVector BaseXDir;
	FVector BaseYDir;
	FPlane BasePlane;//基面

	AActor* mCurrentActor;

	FVector UpVector;
	FVector RightVector;
	FVector ForwardVector;

	FVector mCurrentPosition;

	bool bInRoom;

	// 外面控制参数
	bool bShowXAxis;
	bool bShowYAxis;
	bool bShowZAxis;

	// 2D-3D控制参数
	bool bShowXView;
	bool bShowYView;
	bool bShowZView;

	//根据面控制显示
	bool bShowX;
	bool bShowY;
	bool bShowZ;

	bool bActor;
	//射线起始点
	FVector XFrontStart ;
	FVector XBackStart ;

	FVector YFrontStart ;
	FVector YBackStart ;

	FVector ZFrontStart ;
	FVector ZBackStart ;


	TSharedPtr<"FArmyLine> XFrontLine;
	TSharedPtr<"FArmyLine> XBackLine;

	TSharedPtr<"FArmyLine> YFrontLine;
	TSharedPtr<"FArmyLine> YBackLine;

	TSharedPtr<"FArmyLine> ZFrontLine;
	TSharedPtr<"FArmyLine> ZBackLine;

	TSharedPtr<class SSArmyEditableLabel> XFrontInputBox;
	TSharedPtr<class SSArmyEditableLabel> XBackInputBox;
	TSharedPtr<class SSArmyEditableLabel> YFrontInputBox;
	TSharedPtr<class SSArmyEditableLabel> YBackInputBox;
	TSharedPtr<class SSArmyEditableLabel> ZFrontInputBox;
	TSharedPtr<class SSArmyEditableLabel> ZBackInputBox;
	TSharedPtr<class SBox> border;

	int32 XFrontV = 0;
	int32 XBackV = 0;
	int32 YFrontV = 0;
	int32 YBackV = 0;
	int32 ZFrontV = 0;
	int32 ZBackV = 0;

	int32 PXFrontV=0;
	int32 PXBackV=0;
	int32 PYFrontV=0;
	int32 PYBackV=0;
	int32 PZFrontV=0;
	int32 PZBackV=0;

	int32 XMain=1;
	int32 YMain=1;

	FVector2D Pos;

	AxisInfo XFrontAxis;
	AxisInfo XBackAxis;
	AxisInfo YFrontAxis;
	AxisInfo YBackAxis;
	AxisInfo ZFrontAxis;
	AxisInfo ZBackAxis;

	bool bInView = true;

	EVisibility XShow = EVisibility::Collapsed;
	EVisibility YShow = EVisibility::Collapsed;
	EVisibility ZShow = EVisibility::Collapsed;
};
