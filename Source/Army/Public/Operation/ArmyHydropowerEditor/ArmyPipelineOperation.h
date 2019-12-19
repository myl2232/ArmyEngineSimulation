#pragma once

#include "ArmyOperation.h"
#include "ArmyLine.h"
#include "SArmyInputBox.h"
#include "ArmyEditPoint.h"
#include "ArmyPipeline.h"
#include "ArmyResourceService.h"

class FArmyLine;
class SArmyInputBox;
class FArmyPipeline;
class FArmyPipeGraph;

/**
* 绘制状态
*/
enum EPipeDrawType
{
	PipeDraw_None,
	PipeDraw_2D, //2D 绘制
	PipeDraw_3D, //3d 绘制
	PipeOperate_2D,// 2D编辑
	PipeOperate_3D,// 3D编辑
	PipeDraw_Max,
};

// 交点信息
struct FIntersection
{
	FGuid Line1; // 交线1
	FGuid Line2; // 交线2
	FVector Location ;// 交点
	FVector Normal;//法线垂直线1,2
	FIntersection(const FGuid& InLine1,const FGuid& InLine2,FVector& InLocation,FVector InNormal) :
		Line1(InLine1),
		Line2(InLine2),
		Location(InLocation),
		Normal(InNormal)
	{

	}
	FIntersection(){}

};
// 过桥弯交点信息
struct  FIntersectionStateInfo
{
	//相交点
	TSharedPtr<FIntersection> Intersection;
	//状态，是否生成过桥弯
	bool bState;
	FIntersectionStateInfo(){}
	void AddInstersection(const FGuid& Line1,const FGuid& Line2,FVector& InLocation,FVector& InNormal)
	{
		Intersection = MakeShareable(new FIntersection(Line1,Line2,InLocation,InNormal));
		bState = false;
	}
	void UpdateLocation(FVector& InLocation,FVector& InNormal)
	{
		Intersection->Location = InLocation;
		Intersection->Normal = InNormal;
	}
	void SetState(bool InState){bState = InState;}
	bool GetState(){return bState;}
	bool Contains(const FGuid& LineID)
	{
		return Intersection->Line1 == LineID || Intersection->Line2 == LineID;
	}
	FGuid& GetLine1(){ return Intersection->Line1; }
	FGuid& GetLine2(){ return Intersection->Line2; }
	bool IsIntersection(); // 是否相交
	bool IsWaterPipeLineIntersection(); //两条水管相交
	bool IsWaterElecIntersection(); //水管和电线相交
	bool IsElecIntersection(); //水管和电线相交
	bool IsLineBridge(FGuid& Line);
	TSharedPtr<FArmyPipeline> GetLinePtr(FGuid& InLineID);
	TSharedPtr<FArmyPipeline> GetOtherLinePtr(FGuid& InLineID);
	FGuid GetOtherLineID(FGuid& InLineID);
	bool IsWaterLine(FGuid& InLineID);
};

//线 ， 交点
struct FLineIntersectionInfo
{
	FGuid LineID; // 线ID
	TArray<TSharedPtr<FIntersectionStateInfo>> Intersectiones; //交点集合
	bool operator==(const FLineIntersectionInfo& InOther) const
	{
		return LineID==InOther.LineID;
	}
	void AddIntersertionInfo(TSharedPtr<FIntersectionStateInfo>& Info);

	FLineIntersectionInfo(const FGuid& InLineID):
		LineID(InLineID)
	{

	}
	bool IsWaterLine();
	void GetNearIntersectionstateInfoes(TSharedPtr<FIntersectionStateInfo>& PointInfo,TArray<TSharedPtr<FIntersectionStateInfo>>& OutArray);
};


/**
* 管线绘制助手
*/
class FArmyPipelineOperation : public FArmyOperation
{
public:
	FArmyPipelineOperation(EModelType InBelongModel);
	FArmyPipelineOperation(EModelType InBelongModel,class UWorld* World);
	virtual ~FArmyPipelineOperation ();

	//~ Begin IArmyDrawHelper Interface
	virtual void Init();
	virtual void InitWidget(TSharedPtr<SOverlay> InParentWidget /* = NULL */);
	virtual void Draw (FPrimitiveDrawInterface* PDI,const FSceneView* View) override;
	virtual void BeginOperation (XRArgument InArg = XRArgument ()) override;
	virtual void EndOperation () override;
	virtual void Tick () override;
    virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	virtual void MouseMove (UArmyEditorViewportClient* InViewPortClient,FViewport* ViewPort,int32 X,int32 Y) override;
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient,class FViewport* InViewPort,FKey Key,EInputEvent Event) override;
	//~ End IArmyDrawHelper Interface

	/**
	* 添加管线
	*/
	virtual void Exec_AddPipeline (const FVector& Start,const FVector& End);

	virtual void Update(UArmyEditorViewportClient* InViewPortClient,class FViewport* InViewPort);

	//@郭子阳 强制Update一次
	void ForceUpdate() { Update(nullptr, nullptr); };

	bool IsNumber ();

	/** 是否在绘制 */
	bool IsDrawing ();

	void SetCurrentItem (TSharedPtr<FContentItemSpace::FContentItem> item);

public:
	TSharedPtr<class FArmyPipePoint> MakeShareablePoint(EObjectType InType,FColor Color,FColor ReformColor,bool bTrans = false);

	// 检测所有点是否能够合并(起始点)
	virtual bool MergeTwoPoint(TArray< TSharedPtr<class FArmyPipePoint > > Pointes);

	// 截断线
	virtual bool TruncationLineByPoint (TArray< TSharedPtr<class FArmyPipePoint > > Pointes);

	// 强电获取弱电 ， 弱电获取强电
	virtual void GetObjects(EPipeType Type , TArray<FObjectWeakPtr>& Objects);

	// 生成过桥弯
	virtual bool CreateBridgeBend(TArray<TSharedPtr<class FArmyPipePoint>>& Points);

	/**
	* 更新过桥弯的位置 OR 删除
	*/
	virtual void UpdateBridgeBend(TArray<TSharedPtr<FArmyPipePoint>>& Points);

	/**
	* 能否合并点
	*/
	virtual bool CanPointesMerge (TSharedPtr<class FArmyPipePoint> P1,TSharedPtr<class FArmyPipePoint> P2);

	/**
	*删除选中的物体
	*/
	void OnDeleteSelectionActor (class AActor* Actor);

	/**
	* 删除线
	*/
	void DeleteLines (TArray<TWeakPtr<FArmyPipeline>>& Lines);

	// 三维拖动管线节点
	void  PostEditPointes(TArray<TSharedPtr<FArmyObject>>& Objectes);

	//回退管线位置
	void  PostModifyEditPointes(TArray<TSharedPtr<FArmyObject>>& Objectes,bool bTrans = false);

	//更新商品信息
	void UpdateProductInfo(int32 ComponentID,FString PipeDiameter,FString ObjectIDStr );

	/*
	*	更新商品信息
	*/
	void UpdateProductInfoAfter(FString guidStr,int32 productID);

	/**
	* 操作管线
	*/
	void  EditorApplyTranslationLine (TSharedPtr<FArmyPipeline> PipeLine,const FVector& deltaVector);
	/**
	*获取当前鼠标点位的房间名字
	*/
	FString GetMousePointSpaceName(const FVector& MousePoint);

	/**
	*判断是否在厨房，卫生间
	*/
	bool IsInKitchenes(const FVector& MousePoint);

	/**
	* 判断两线是否已经相交
	*/
	bool IsLineIntersection(const FGuid& line1 , const FGuid & line2,TSharedPtr<FIntersectionStateInfo>& IntersectionInfo);

	/**
	* 获取过桥弯点信息
	*/
	void GetBridgePointInfo(FGuid& InLineID,TArray<TSharedPtr<FBridgePointInfo>>& OutPointInfoes);
	/**
	* 添加交点
	*/
	void AddIntersection(const FGuid& Line,TSharedPtr<FIntersectionStateInfo>& IntersectionInfo);

	/**
	* 添加管线，到HomeData数据之后
	*/
	void OnObjectOperation(const XRArgument& InArg, FObjectPtr InObj,bool bTransaction=false);

	//移动管线actor
	void OnActorMoved(AActor* Actor);
private:
	//
	void OnInputBoxCommitted (const FText& InText,const ETextCommit::Type InTextAction);
	void OnInputBoxKeyDown(const FKeyEvent& InKeyEvent);
	TSharedPtr<FArmyResourceService> LocalModelService;
protected:

	XRArgument CurrentArg;

	UPROPERTY()
	class UWorld* OwningWorld;

private:
	TWeakPtr<FArmyPipeGraph>  PipeGraph; // 图，没用
	TSharedPtr<SArmyInputBox> InputBoxWidget; //输入框

	TSharedPtr<FArmyLine> UpLeftDashLine;
	TSharedPtr<FArmyLine> UpRightDashLine;
	TSharedPtr<FArmyLine> UpDashLine;


	//操作的三维线段
	TSharedPtr<FArmyPipeline> SolidPipeLine;

	TSharedPtr<class FArmyRulerLine> RulerLine;

	TSharedPtr<class FArmyAuxiliary> Auxiliary;

	FVector Start;
	FVector End;

	float Radius;

	FColor Color;

	FVector MousePosition;

	bool bShowCash; // 是否显示交接点

	UMaterialInstanceDynamic* MaterialInsDy;

	TSharedPtr<FContentItemSpace::FContentItem> ContentItem;

	TArray<FVector> CachedPoints;

	// 前一个点是否在卫生间之类的
	int32 bPreIsInKitnes = -1; // -1 初始状态 ；0 不太厨房；1 在厨房 

	/**交点矩阵 key LineID , value 交点信息*/
	TArray<FLineIntersectionInfo> IntersectionInfoes;
};