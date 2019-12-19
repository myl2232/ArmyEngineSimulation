/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRHardModeController.h
 * @Description 硬装模式视图控制器
 *
 * @Author 欧石楠
 * @Date 2018年6月11日
 * @Version 1.0
 */

#pragma once

#include "ArmyHttpModule.h"
#include "ArmyDesignModeController.h"
#include "ArmyApplyMasterPlanManage.h"
#include "ArmyPackageDownloader.h"

enum HardModeEditMode
{
	HO_FreeEdit,
	HO_FloorEdit,
	HO_WallEdit,
	HO_RoofEdit
};
class FArmyHardModeController : public FArmyDesignModeController, public TSharedFromThis<FArmyHardModeController>
{
public:

	friend class FArmyMaterialBrushOperation;

	~FArmyHardModeController();

	//~ Begin FArmyDesignModeController Interface
    virtual const EModelType GetDesignModelType() override;
	virtual void Init() override;
	virtual void InitOperations() override;
	virtual void BeginMode() override;
	virtual bool EndMode() override;
	virtual void EndOperation() override;
	virtual TSharedPtr<SWidget> MakeLeftPanelWidget() override;
	virtual TSharedPtr<SWidget> MakeRightPanelWidget() override;
	virtual TSharedPtr<SWidget> MakeToolBarWidget() override;
	virtual TSharedPtr<SWidget> MakeSettingBarWidget() override;
	virtual void Draw(FViewport* InViewport, const FSceneView* InView, FCanvas* SceneCanvas) override;
	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI);
	virtual bool InputKey(FViewport* Viewport, FKey Key, EInputEvent Event) override;

	virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Clear() override;
	virtual void Load(TSharedPtr<FJsonObject> Data) override;
	virtual bool Save(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter) override;
	virtual void Delete() override;
	//~ End FArmyDesignModeController Interface

	void ReplaceTextureOperation();

	void CameraReturnCallBack();

	void DisplyOrHidden(bool bDisplay);

	void OnObjectOperation(const XRArgument& InArg, FObjectPtr InObj,bool bTransaction=false);

	virtual void CollectAllLinesAndPoints(TArray< TSharedPtr<FArmyLine> >& InOutLines, TArray< TSharedPtr<FArmyPoint> >& InOutPoints, TArray<TSharedPtr<FArmyCircle>>& InOutCircles);

public:
	void LockCameraToEditSurface();
	void OnSaveCommand();
	void OnUndoCommand();
	void OnRedoCommand();
	//@ 郭子阳 材质刷功能
	void OnMaterialBrushCommand();
	/** @马云龙 清空场景中的方案命令 */
	void OnEmptyPlanCommand();
	/** @马云龙 清空光照烘焙数据命令 */
	void OnEmptyStaticLightingCommand();
	void OnDeleteCommand();
	void OnReplaceCommand();
	void OnCopyCommand();
	/** @马云龙 构建光照命令 */
	void OnBuildStaticLightingCommand();

	/** @马云龙 下拉菜单按钮。立面模式下清空分为两种，一种是清空场景中的方案，一种是清空光照烘焙数据 */
	TSharedRef<SWidget> OnEmptyCommand();
	TSharedRef<SWidget> OnLightCommand();


    TSharedRef<SWidget> OnDisplayCommand();
	/**@欧石楠 平面视图下拉*/
	TSharedRef<SWidget> OnViewComboCommand();
	bool ShouldTick();
	// 常远 方法从私有变成公开
	void On3DTo2DGanged(TSharedPtr<FContentItemSpace::FComponentRes> InRes, AActor* InActor, EModeIdent InModeIdent);

	//@郭子阳 当执行全局命令时发生的时间
	void OnGloabalAction();
public:
	/**施工项数据获取完毕的回调*/
	//void OnConstructionWallReady();
	//void OnConstructionRoofReady();
	//void OnConstructionFloorReady();

	//void ReqConstructionData(int32 GoodsId);
	//void ResConstructionData(FArmyHttpResponse Response, int32 GoodsId);

	/** @打扮家 XRLightmass 保存渲染参数*/
	void SaveWorldSettings(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter);
	void LoadWorldSettings(TSharedPtr<FJsonObject> Data);
	/** @打扮家 XRLightmass 保存光照数据包文件*/
	bool SaveMapBuildData(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter);
	/** @打扮家 XRLightmass 加载光照数据包文件*/
	void LoadMapBuildData(FString InFilePath);
	/** @打扮家 XRLightmass 尝试加载，加载失败提示*/
	void TryLoadMapBuildData();
	/** @打扮家 XRLightmass 切换动静光照模式 */
	//void SetStaticLightEnvironment();
	//void SetDynamicLightEnvironment();

	HardModeEditMode CurrentEditMode = HO_FreeEdit;
protected:
	TArray<FString> HideActorList;
	void ResetDefaultStyle();

	/** @马云龙 构建光照 */
	void BuildStaticMeshLighting(int32 InQuality);
	/** @马云龙 构建成功/失败 */
	void BuildStaticLightingFinished(bool bInSuccess);
	/** @马云龙 执行清空场景中的光照烘焙数据 */
	void ClearStaticLighitngData();
	/** @马云龙 清空/生成用于烘焙光照放置漏光的门洞遮挡物体 */
	void ClearDoorBlocks();
	void GenerateDoorBlocks();
	/** @马云龙 方案已载入完毕的回调 */
	void AllControllersLoadCompleted();
	/** @马云龙 每次切换模式后，调用BeginMode，经过几次Tick之后会调用该函数，用来调用比如延迟更新反射球 */
	void DelayUpdate();
	/** @马云龙 更新反射 */
	void UpdateReflection();
	/** @马云龙 户型修改重建3D数据的回调 */
	void OnHomeRebuild();

	UMaterialInstanceDynamic* MI_BackGroundColor;
	FVector LastCameraViewLocation;
	FRotator LastCameraViewRotator;
	TSharedPtr<class FArmyHardModeFloorController> HardModeFloorController;
	TSharedPtr<class FArmyHardModeWallController> HardModeWallController;
	TSharedPtr<class FArmyHardModeCeilingController> HardModeCeilingController;
	FVector2D ViewportSize;
	
	//瀑布流目录管理
	TSharedPtr<class SArmyCategoryOrganizationManage> CategoryOrgManage;
	FVector2D LastViewportMousePos;
	FVector2D ViewportMousePos;


	TArray<FVector> SurfacePoints;
	TSharedPtr<FArmyRoomSpaceArea> CurrentEditSurface;

	//DetailBuilder结构
	TSharedPtr<class FArmyDetailBuilder> DetailBuilder;

	TSharedPtr<class FArmyAxisRulerActor> ActorAxisRuler;

	/**@马云龙 里面模式下的按键命令 */
	TSharedPtr<FUICommandList> HardModeGlobalCommandList;
	TSharedPtr<class SArmyHardModeContentBrowser> LeftPanel;
public:
	//属性面板
	TSharedPtr<class FArmyHardModeDetail> HardModeDetail;
private:
	class AXRShapeActor* HightLightActor;

	/**@马云龙 鼠标当前检测到的交互物体 */
	AActor* TempInteractiveActor;
	/**@马云龙 每次切换模式后调用BeginMode，都会重置0，Tick一定的次数回去调用DelayPostBeginMode */
	int32 DelayPostBeginModeTickCount;

	/**
	* 清空软装数据
	* @param bClearResource - bool - 是否清空模型资源
	* @param bClearRoomSpace - bool - 是否清空各面（墙面、顶面、地面）资源
	*/
	void Cleanup(bool bClearResource, bool bClearRoomSpace);
	/**@欧石楠绑定右键代理*/
	void OnRightSelected();
	/**@欧石楠右键菜单*/
	void ShowRightMenu(bool bShow);
	void OnActorMoved(AActor* Actor);
	void FArmyHardModeController::OnViewModeChanged(int32 InNewValue, int32 InOldValue);
	void OnSelectionChanged(UObject* NewSelection);
	/** @欧石楠 未选中任何物体的回调 */
	void OnSelectionNone();
	//模型数据的保存
	void SaveModel(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter , TArray<int32>& Save_FileList);
	void LoadModel(TSharedPtr<FJsonObject> Data);
	
	void ThreeTo2DGanged(TSharedPtr<FContentItemSpace::FComponentRes> InRes,AActor* InActor,TSharedPtr<FArmyObject> Object = nullptr) override;
	void GangedTransformUpdated(USceneComponent* InRootComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport);

	void OnPointLightCommand();
	void OnSpotLightCommand();
	void OnReflectionCaptureCommand();
    void OnDisplayLightIconCommand();
    bool IsDisplayLightIconChecked();

	void LockCamera(const XRArgument& InArg, FObjectPtr editArea, bool bTransaction/*=false*/);
	bool LockCameraToEditSurface(TSharedPtr<FArmyObject> editArea);
	bool SaveFileListID(const TSharedPtr<FArmyObject> obj)const;

	//@ 计算模型和构件距离内墙最小距离(计算构件距离内墙最小值需要模型计算出的最小内墙线)
	bool CalModelMinDis(AActor* InputActor,
		TSharedPtr<FArmyFurniture> ComponentObj,
		TMap < FString/*方向*/, TMap<TSharedPtr<FArmyLine>/*内墙线*/, TSharedPtr<FArmyLine>/*最短距离线*/ > > &InputmapXYMinDis,
		TMap < FString/*方向*/, TMap<TSharedPtr<FArmyLine>/*内墙线*/, TSharedPtr<FArmyLine>/*最短距离线*/ > > &OutmapXYMinDis);
	//@ 计算包围盒到边的最小距离
	bool CalPointToLineDis(FBox ActorBox, TSharedPtr<FArmyLine>line ,
		TMap < FString, TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine> > > &OutmapXYMinDis,TArray<float> & XYMinArr);

	/**@马云龙 实时检测可交互物体 */
	void TickTraceInteractiveActors();
	/**@马云龙 按空格键执行交互行为 */
	void OnInteractiveHovered();
	

    float GetOutlinerWidgetHeight() const;
    float GetPropertyWidgetHeight() const;

	/**@常远 初始化颜值包中模型的基本信息 */
	void InitDesignPackageMap(TArray<TSharedPtr<class FContentItemSpace::FContentItem>> InContentItemList, TMap<int32, TSharedPtr<class FArmyDesignPackage>> InDesignPackageMap);
	// 应用大师方案相应函数
	void OnApplayMasterPlan(TSharedPtr<FContentItemSpace::FContentItem> InContentItem,/*FArmyApplyMasterPlanManage::*/ApplayExtentS InApplayExtent);
	// 应用大师方案，不包含下载
	void OnApplayMasterPlanNoDownload(bool bDownloadSuccess, TSharedPtr<FContentItemSpace::FContentItem> InContentItem, ApplayExtentS InApplayExtent);
private:
	// 颜值包下载器
	TSharedPtr<class FArmyPakageDownloader> Downloader;

	// 颜值包
	TMap<int32,TSharedPtr< class FArmyDesignPackage>> DesignPackageMap;

    /** 是否显示光源图标 */
    bool bIsDisplayLightIconChecked = true;

    /** 右侧物体列表和属性面板高度 */
    float OutlinerWidgetHeight = 280.f / 1012.f;
    float PropertyWidgetHeight = 732.f / 1012.f;

	/** @马云龙 从方案Json中提取的渲染文件路径，如果为空，说明方案中未关联渲染文件 */
	FString LoadedLevelDataFilePath = "";

	/** @马云龙 最低支持的渲染文件版本号，低于此版本号的光照文件将不被兼容，加载方案的时候会自动删除 */
	const static int32 LightmassLastSupportedVersion = 160;

	/* @梁晓菲 控制光源图标显隐*/
	void SetLightsVisibility(EVisibility LightVisible);

	//在门的位置生成隐藏的盒子，放置漏光
	TArray<class AXRShapeActor*> DoorBlocks;

public: 
	//获取被选中的面
	TSharedPtr<FArmyBaseArea> GetSelectedArea() {	return SelectedArea;};
	private:
		TSharedPtr<FArmyBaseArea> SelectedArea=nullptr;
};