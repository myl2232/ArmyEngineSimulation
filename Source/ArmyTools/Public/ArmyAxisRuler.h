#pragma once
#include "CoreMinimal.h"
//#include "ArmyObject.h"

DECLARE_DELEGATE_RetVal_ThreeParams(bool, F"FArmyObjectDelegate, FObjectPtr, FTransform,int32);

class ARMYTOOLS_API "FArmyAxisRuler :
	public TSharedFromThis<"FArmyAxisRuler>
{
public:
	"FArmyAxisRuler();
	~"FArmyAxisRuler();

	void Init(TSharedPtr<SOverlay> ParentWidget);

	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	void UpdateTransform();

	void Update(UArmyEditorViewportClient* InViwportClient, const TArray<FObjectWeakPtr>& InObjects);

	void Update2D(UArmyEditorViewportClient* InViwportClient, const TArray<FObjectWeakPtr>& InObjects);
	void Update3D (UArmyEditorViewportClient* InViwportClient, const TArray<FObjectWeakPtr>& InObjects);

	/**	设置绑定对象*/
	void SetBoundObj(FObjectPtr InObj);

	FObjectPtr GetBoundObj() { return CurrentObj; };

	void SetRefCoordinateSystem(const FVector& InBasePos, const FVector& InXDir, const FVector& InYDir, const FVector& InNormal);

	const FPlane& GetPlane() const;

	void SetSceneModel(int32 InSceneModel);

	float GetXDirDistance() { return XProjectionDistance; }
	float GetYDirDistance() { return YProjectionDistance; }

	FVector GetXDirCenter() { return (XFrontLine->GetEnd() + XBackLine->GetEnd()) / 2.f; }
	FVector GetYDirCenter() { return (YFrontLine->GetEnd() + YBackLine->GetEnd()) / 2.f; }

	/**@刘克祥 自动对边吸附*/
	void AutoAttach();

	void SetEnableAutoAttach(bool bEnable) { bEnableAutoAttach = bEnable; }

	//@刘克祥
	//设置在绘制区域中使用
	void SetUseInDrawArea(bool Value);

	//@刘克祥 
	//输入标尺的边界，标尺不会超出边界
	void SetBound(TArray<FVector> & InOutLine) {OutLine = InOutLine;};
	//@刘克祥 
	//输入标尺的圆形边界，标尺不会超出边界
	void SetBound(FVector Center, float radius) 
	{
		CircleCenter = Center;
		CircleRadius = radius; 
		UseCircleBound = true;
	};
	//@刘克祥
	//禁用圆形边界
	void DisableCircleBound()
	{
		UseCircleBound = false;
	}

public:
	F"FArmyObjectDelegate AxisOperationDelegate;
private:

	//正则
	bool CheckStringIsVaild(const FString& str);

	void OnTextCommitted(const FText&, const SWidget*);

	FVector ToBasePlane(const FVector& InV);
	FVector FromBasePlane(const FVector& InV);

	bool UseInDrawArea = false;

	//@刘克祥
	//多边形边界
	TArray<FVector>  OutLine;

	//@刘克祥
	//圆形边界的中心
	FVector CircleCenter;
	float CircleRadius;
	bool UseCircleBound = false;

private:
	FVector BasePoint;
	FVector BaseNormal;
	FVector BaseXDir;
	FVector BaseYDir;
	FPlane BasePlane;//基面

	FObjectPtr CurrentObj;
	uint32 SceneModel;//场景模式，0 ： 2D 1：3D

	int32 XMain = 1;
	int32 YMain = 1;

	TSharedPtr<"FArmyLine> XDrawFrontLine;
	TSharedPtr<"FArmyLine> XDrawBackLine;

	TSharedPtr<"FArmyLine> YDrawFrontLine;
	TSharedPtr<"FArmyLine> YDrawBackLine;

	TSharedPtr<"FArmyLine> ZDrawFrontLine;
	TSharedPtr<"FArmyLine> ZDrawBackLine;

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

	float XFrontV = 0;
	float XBackV = 0;
	float YFrontV = 0;
	float YBackV = 0;
	float ZFrontV = 0;
	float ZBackV = 0;

	/**@刘克祥 2D下 X或者Y方向的两个投影点的距离*/
	float XProjectionDistance = 0;
	float YProjectionDistance = 0;
	/**@刘克祥 是否启用自动吸附*/
	bool bEnableAutoAttach = false;
};
