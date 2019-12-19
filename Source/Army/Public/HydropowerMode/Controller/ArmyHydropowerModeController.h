/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRHardModeController.h
 * @Description 水电模式视图控制器
 *
 * @Author 朱同宽
 * @Date 2018年6月11日
 * @Version 1.0
 */

#pragma once

#include "ArmyDesignModeController.h"
#include "ArmyObjectVisualizerManager.h"
#include "SArmyObjectTreeItem.h"
#include "ArmyHttpModule.h"
#include "ArmyConstructionManager.h"
class FArmyPipePoint;
class FArmyPipeline;


class FArmyHydropowerModeController : public FArmyDesignModeController, public TSharedFromThis<FArmyHydropowerModeController>
{
public:
	enum EHydropowerModeOperation
	{
		HO_NONE = 0,
		HO_DrawLine = 5,// 绘制管线
		HO_Component = 4, //添加点位
		HO_SwitchConnect = 3, //开关控制图
		HO_PowerSystem = 6,//配电箱控制系统
	};
	/**
	* 清空操作
	*/
	enum EEmtpy
	{
	  	EE_None,
		EE_Strong,//强电删除
		EE_Light,//照明删除
		EE_Weak,//弱点删除
		EE_ColdWater,//冷水管删除
		EE_HotWater,//热水管删除
		EE_Drain,//排水管删除
		EE_Max,//全部
	};
public:
    ~FArmyHydropowerModeController();

    //~ Begin FArmyDesignModeController Interface
    virtual const EModelType GetDesignModelType() override;
	virtual void Init() override;
	virtual void InitOperations () override;
    virtual void BeginMode() override;
    virtual bool EndMode() override;
    virtual TSharedPtr<SWidget> MakeLeftPanelWidget() override;
    virtual TSharedPtr<SWidget> MakeRightPanelWidget() override;
    virtual TSharedPtr<SWidget> MakeToolBarWidget() override;
    virtual TSharedPtr<SWidget> MakeSettingBarWidget() override;
	virtual void Draw (FViewport* InViewport,const FSceneView* InView,FCanvas* SceneCanvas) override;
	virtual void Draw (const FSceneView* View,FPrimitiveDrawInterface* PDI);
	virtual bool InputKey (FViewport* Viewport,FKey Key,EInputEvent Event) override;
	virtual bool InputAxis (FViewport* Viewport,int32 ControllerId,FKey Key,float Delta,float DeltaTime,int32 NumSamples = 1,bool bGamepad = false) override;
	virtual void Tick (float DeltaSeconds) override;
	virtual void MouseMove (FViewport* Viewport,int32 X,int32 Y) override;
    virtual void Clear() override;
    virtual void Load(TSharedPtr<FJsonObject> Data) override;
    virtual bool Save(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter) override;
	virtual void Delete() override;
	void OnActorMoved(AActor* Actor);
	virtual void EndOperation() override;
	virtual bool AxisOperation(FVector direction);
    //~ End FArmyDesignModeController Interface

	virtual void Empty(int32 IntVal);

	//virtual bool CanEmpty(int32 IntVal,int32)

	TSharedRef<SWidget> OnEmptyCommandes();
	/**@欧石楠 平面视图下拉*/
	TSharedRef<SWidget> OnViewComboCommand();
	/**
	* 清除管线资源
	*/
	void Cleanup (bool bClearResource);


	bool ShouldTick();
	bool IsMiddleButtonPressed() const;

	//选择当前商品（管线，点位）
	void SelectContentItem();	


	// 绑定物体选中事件
	/** 未选中任何物体的回调 */
	void OnSelectionNone ();

	/** 选中物体后的回调 */
	void OnSelectionChanged (UObject* NewSelectionObject);

	/**删除选中的物体*/
	void OnDeleteSelectionActor (AActor* Actor);

	/**Copy选中的物体*/
	void OnCopySelectionActorToNew (AActor* OldActor,AActor* NewActor);

	/**添加点位*/
	void OnLevelActorsAdded (AActor* InActor);

	/**下载完毕构件之后生成3D模型*/
	virtual void On3DTo2DGanged (TSharedPtr<FContentItemSpace::FComponentRes> InRes,AActor* InActor,EModeIdent InModeIdent);

	/**移动3D模型修改2D数据*/
	void GangedTransformUpdated (USceneComponent* InRootComponent,EUpdateTransformFlags UpdateTransformFlags,ETeleportType Teleport);

	/**放入点位时操作*/
	void ProjectActorsIntoWorld (const TArray<AActor*>& Actors);

	void PlaceHydropowerActorIntoWorld(const TArray<AActor*>& Actors,FVector directNormal);

	/**根据生成的3D模型生成2D的dxf*/
	void ThreeTo2DGanged(TSharedPtr<FContentItemSpace::FComponentRes> InRes,AActor* InActor,TSharedPtr<FArmyObject> Object = nullptr) override;

	//@郭子阳
	//根据3d模型和无构件商品生成Furniture
	void CreateObjForMeshActor(FContentItemPtr ContentItem, AActor* InActor, TSharedPtr<FArmyObject> Object=nullptr, EModeIdent Mode= MI_None);

	/**
	* 持续添加点位
	*/
	void AddToScene( );

public:
	/**保存命令*/
	void OnSaveCommand();
	/**撤销命令*/
	void OnUndoCommand();
	/**进入灯控设置*/
	void OnChangeToSwtichConnectCommand(int32 bEnter);
	/**重做*/
	void OnRedoCommand();
	/*清空*/
	void OnEmptyCommand();

	//当前模式是否是灯控设置模式
	bool IsSwitchConnect();

	/*判断是否能显示灯控设置*/
	bool CanVisibleSwitchConnect();
	//删除强电
	void OnEmptyStrongCommand();
	//删除照明
	void OnEmptyLightCommand();
	//删除弱点
	void OnEmptyWeakCommand();
	//删除冷水管
	void OnEmptyColdWaterCommand();
	//删除热水管
	void OnEmptyHotWaterCommand();
	//删除排水管
	void OnEmptyDrainCommand();
	void OnDeleteCommand();
	//进入配电系统
	void OnEnterPowerSystem();

	//智能配电
	void OnDesignPipeLine();
	void OnReplaceCommand();
	/*copy命令*/
	void OnCopyCommand();
	/**进入灯控设置图*/
	void BeginSwitchOperation(bool bWasSucceed);

	//@ 智能布点(开关、插座、水位)
	void OnAutoDesignPoint();

private:
	/**@欧石楠视图模式切换*/
	void OnViewModeChanged(int32 InNewValue, int32 InOldValue);

private:
	/**右侧列表*/
	TSharedRef<SWidget> CreateLayerOutliner();
	/**右侧属性列表*/
	TSharedRef<SWidget> CreateDetail();
	/**创建开关控制图*/
	TSharedRef<SWidget> CreateSwitchConnect();

    float GetOutlinerWidgetHeight() const;
    float GetPropertyWidgetHeight() const;

private:
	//配电箱系统设置
	TSharedPtr<class FArmyPowerSystemManager> PowerSystemManager; 
	//属性界面
	TSharedPtr<class FArmyHydropowerDetail> HydropowerDetail;
	//开关控制图
	TSharedPtr<class FArmyHydropowerSwitchConnect> HydropowerSwitchConnect;
	//六轴坐标
	TSharedPtr<class FArmyAxisRuler3D> AxisRuler;

	TSharedPtr<class SVerticalBox> LayerOutliner1;
	TSharedPtr<class SVerticalBox> LayerOutliner2;
	//控件灯控设置 图标
	TSharedPtr<class SWidgetSwitcher> MyWidgetSwitcher;
	FVector2D ViewportSize;
	FVector2D LastViewportMousePos;
	FVector2D ViewportMousePos;
	TSharedPtr<class SArmyHydropowerModeContentBrowser> LeftPanel;

	//瀑布流目录管理
	TSharedPtr<class SArmyCategoryOrganizationManage> CategoryOrgManage;

	TArray<uint32> TopViewIgnoreObjectArray;

    /** 右侧物体列表和属性面板高度 */
    float OutlinerWidgetHeight = 280.f / 1012.f;
    float PropertyWidgetHeight = 732.f / 1012.f;

	bool bViewTOP = false;// 是否是顶视图
	UMaterialInstanceDynamic* MaterialInsDy;
	TSharedPtr<SOverlay> OverlyView;

	// 当前OperationID
	int32 CurrentOperationID;

	int32 GetCurrentOperationID();

public:
	bool IsAdded = false;

	/* @梁晓菲 控制光源图标显隐*/
	void SetLightsVisibility(EVisibility LightVisible);

public:
	//***********************断点改造相关*************************************

	/*替换操作
	* @NewContentItem 新的商品
	* @ ReplacedFurniture 被替换的构件
	*/
	void Replace(FContentItemPtr NewContentItem, TSharedPtr<FArmyFurniture> ReplacedFurniture);

	//停用点位
	void StopOriginalPoint(TSharedPtr<FArmyFurniture> StoppedFurniture);
	//启用用点位
	void StartOriginalPoint(TSharedPtr<FArmyFurniture> StoppedFurniture);
	//施加偏移
	void ApplyOffset(TSharedPtr<FArmyFurniture> OffsetFurniture ,FVector offset);

	//开启断点改造
	void StartBPReform(TSharedPtr<FArmyFurniture> StoppedFurniture);
	//关闭断点改造
	void StopBPReform(TSharedPtr<FArmyFurniture> StoppedFurniture);

	//显隐构件actor
	void OnFurnitureActorShowUpdated(TSharedRef<FArmyFurniture> Furniture, bool NewHiden);

	//***********************断点改造结束*************************************

	//***********************施工项相关***************************************
public:


	//发送获取施工项的请求，用于非纯移位
	 bool RequestConstructionList(AActor * Actor);

	//发送获取施工项的请求，用于纯移位
	bool RequestConstructionList(TSharedPtr<FArmyFurniture> Furniture);

	//根据构件类型获得施工项后台参数
	static int32 GetConstructionID(TSharedPtr<FArmyFurniture> Furniture);

	//获得构件施工项相应
	void OnResConstructionData(FArmyHttpResponse Response,int32 GoodsID);

	//DECLARE_DELEGATE_OneParam(FHyConstructionDataDelegate, TSharedPtr<FJsonObject>);

	FConstructionDataDelegate OnConstructionDataGot;

	//获取施工项的回调
	//@ConstructionData 施工项原始数据
	//@CheckedData 勾选数据
	void ConstructionCallBack(enum class EConstructionRequeryResult ResultInfo, TSharedPtr<FJsonObject> ConstructionData, TSharedPtr<class FArmyConstructionItemInterface> CheckedData, ObjectConstructionKey Key);
}; 