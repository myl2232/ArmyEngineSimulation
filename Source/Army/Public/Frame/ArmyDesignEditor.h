/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRDesignEditor.h
 * @Description 设计界面框架
 *
 * @Author 欧石楠
 * @Date 2018年6月7日
 * @Version 1.0
 */

#pragma once

#include "AliyunOss.h"
#include "DownloadTypes.h"
#include "ArmyViewController.h"
#include "ArmyDesignModeController.h"
#include "SArmyDesignFrame.h"
#include "SArmyDesignTitleBar.h"
#include "ArmySceneOutlinerModule.h"
#include "ResTypes.h"
#include "IHttpRequest.h"

 /** 资源版本 */
const static int32 GResourceVersion = 10001;

DECLARE_MULTICAST_DELEGATE_OneParam(BaseDataChangeCallBack, int32);

namespace DesignMode
{
	const static FString HomeMode = TEXT("原始户型");
	const static FString LayoutMode = TEXT("拆改户型");
	const static FString HardMode = TEXT("立面模式");
	const static FString HydropowerMode = TEXT("水电模式");
	const static FString WHCMode = TEXT("木作模式");
	const static FString ConstrctionMode = TEXT("图纸模式");
}

struct FMoveableMeshSaveData
{
	int32 ItemID;
	FVector Location;
	FRotator Rotation;
	FVector Scale3D;
	TArray<FMeshMaterialData> MaterialList;
	//-1 不可Toggle  0 关  1 开  2...N开的不同状态
	int32 DefaultToggleMode;
	TSharedPtr<FArmyObject> Object;
	//@ 自动设计数据标记，0为非自动设计，1 代表开关 ，2代表插座 ，3代表水位
	int32 AutoDesignFlag;

	FMoveableMeshSaveData(int32 InItemID, FVector InLocation, FRotator InRotation, FVector InScale3D, TSharedPtr<FArmyObject> InObject, int32 InDefaultToggleMode = -1, int32 InAutoDesignFlag = 0)
	{
		ItemID = InItemID;
		Location = InLocation;
		Rotation = InRotation;
		Scale3D = InScale3D;
		Object = InObject;
		DefaultToggleMode = InDefaultToggleMode;
		AutoDesignFlag = InAutoDesignFlag;
	}

	FMoveableMeshSaveData(int32 InItemID, FVector InLocation, FRotator InRotation, FVector InScale3D, int32 InDefaultToggleMode = -1, int32 InAutoDesignFlag = 0)
	{
		ItemID = InItemID;
		Location = InLocation;
		Rotation = InRotation;
		Scale3D = InScale3D;
		Object = nullptr;
		DefaultToggleMode = InDefaultToggleMode;
		AutoDesignFlag = InAutoDesignFlag;
	}
};

class FArmyDesignEditor : public FArmyViewController, public TSharedFromThis<FArmyDesignEditor>
{
public:
	FArmyDesignEditor();
	~FArmyDesignEditor();

	void PreInit();

	//~ Begin FArmyViewController Interface
	virtual void Init() override;
	virtual TSharedPtr<SWidget> MakeCustomTitleBarWidget() override;
	virtual TSharedPtr<SWidget> MakeContentWidget() override;
	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual void Draw(FViewport* InViewport, const FSceneView* View, FCanvas* SceneCanvas) override;
	virtual bool InputKey(FViewport* Viewport, FKey Key, EInputEvent Event) override;
	virtual bool InputAxis(FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples = 1, bool bGamepad = false) override;
	virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void MouseMove(FViewport* Viewport, int32 X, int32 Y) override;
	virtual bool MouseDrag(FViewport* Viewport, FKey Key) override;
	virtual TSharedPtr<FUICommandList> GetCommandList() override;
	//~ End FArmyViewController Interface

	static FObjectWeakPtr GetObjectByGuidAlone(const FGuid& InGuid);

	/** 基础数据发生变化（目前只有判断户型数据的变化 InDataType = 0 表示户型数据）*/
	void OnBaseDataChanged(int32 InDataType = 0);

	/** 重新加载设计界面，例如新建方案/打开方案时调用 */
	void Reload();

	/** 清空所有 */
	void ClearAll();

	/** 获取所有的模式名字集合 */
	const TArray< TSharedPtr<FString> > GetModeNameArray();

	/** 设置当前设计模式 */
	bool SetCurrentDesignMode(const FString& ModeName);

	/**@欧石楠 获取当前模式名称*/
	FString GetCurrentDesignMode() { return CurrentModeString; }

	/**@欧石楠 获取当前模式名称*/
	FString GetCheckDesignMode() { return CheckModeString; }

	/** @马云龙 获取某个Controller */
	template<class ControllerType>
	TSharedPtr<ControllerType> GetModeController(FString InModeName)
	{
		for (auto& It : ViewControllers)
		{
			if (It.Key == InModeName)
			{
				return StaticCastSharedPtr<ControllerType>(It.Value);
			}
		}

		return nullptr;
	}


    /**
	 * 收集方案数据
	 * @param OutHomeData - TArray<uint8> & - 户型数据
	 * @param OutPlanData - TArray<uint8> & - 方案数据
	 */
    bool CollectPlanData(TArray<uint8>& OutHomeData, TArray<uint8>& OutPlanData);

	/** 保存方案 */
	void SavePlan();

    /** @欧石楠 另存方案 */
    void SaveAsPlan();

	/** 载入户型 */
	void LoadHome(const FString& FilePath);

	/** 载入方案 */
	void LoadPlan(const FString& FilePath);

	/** 方案同步 */
	void ReqSyncPlan(const FString& FilePath);
	void ResSyncPlan(struct FArmyHttpResponse Response, FString InIds);
	FBoolDelegate OnSyncFinished;

	//*********方案估价相关*************
	//方案估价
	void OnShowPlanPrice();

	/**@郭子阳 导出施工用料清单,但不保存 */
	void ExportConstructionListWithoutSave(FOnUploadComplete OnUploaded);
private:
	void OnPlanPriceResponsed(FArmyHttpResponse Response);
	TSharedPtr<class SArmyPlanPrice> PlanPriceUI;

public:
	/** 导出施工用料清单 */
	void ExportConstructionList();

	/** 开启默认的捕捉模式 */
	void OpenDefaultCaptureModel();

	/** 更新同步进度 */
	void UpdateSyncProgress();

	/** 加载默认模型 */
	void LoadCommonModel(const FString& PakFileName, const FString& JsonFileName, const int32 ItemId);

	/** 加载公有资源 */
	void LoadCommonResource();

    /** @欧石楠 截取户型缩略图 */
    void TakeHomeScreenshot();

    /** @欧石楠 退出设计界面后调用 */
    virtual void Quit() override;

private:
	/**	构建构建三维场景物体列表*/
	void ConstructWorldOutliner();

	void ResUploadConstructionQuantityFile(TSharedPtr<FAliyunOssResponse> Response);

	void ReqSubmitConstructionQuantity(const FString& CQFileUrl);
	void ResSubmitConstructionQuantity(struct FArmyHttpResponse Response);

public:
	/** 当前操作的控制器 */
	TSharedPtr<FArmyDesignModeController> CurrentController;

	TSharedPtr<IArmySceneOutliner> WorldOutliner;

	TArray<int32> SaveFileList;

	/** 同步的数据 */
	TArray<FContentItemPtr> SynContentItems;

	/** 创建3D户型模型的代理 */
	FSimpleDelegate OnCreateHomeModelDelegate;

	/** 拷贝原始户型数据的代理 */
	FSimpleDelegate OnCopyHomeDataDelegate;

	TWeakPtr<SArmyDesignTitleBar> DesignTitleBar;

    TSharedPtr<SArmyDesignFrame> DesignFrame;

	/** 是否正在重新加载界面 */
	bool bReloading;

	/** @马云龙 所有PC都加载完毕，场景中所有2D/3D数据已经创建完毕 */
	FMultiVoidDelegate AllControllersLoadCompleted;

private:
	BaseDataChangeCallBack OnBaseDataChangedDelegate;

	TMap< FString, TSharedPtr<FArmyDesignModeController> > ViewControllers;

	/**@欧石楠 记录当前模式名称*/
	FString CurrentModeString;

	/**@欧石楠 记录检查是否可以切换模式时的名称*/
	FString CheckModeString;

	/** 施工算量json文件名 */
	FString ConstructionQuantityFileName;

	/** 同步相关 */
	FText GetSyncProgressText() const { return FORMAT_TEXT("同步方案中%d%%...", FMath::FloorToInt(SyncProgress)); }
	TSharedPtr<class FDownloadFileSet> SyncTasks = nullptr;
	float SyncProgress = 0.f;
    TArray<FDownloadSpace::FDownloadFileInfo> NeedToUpdateFiles;
    TArray<FString> IdsArray;

    /** @欧石楠 保存方案 */
    TSharedPtr<class SArmySavePlan> SavePlanWidget = nullptr;

    /** @欧石楠 另存方案 */
    TSharedPtr<class SArmySaveAsPlan> SaveAsPlanWidget = nullptr;

    /** @欧石楠 自动保存提示 */
    TSharedPtr<class FArmyAutoSave> AutoSave;
};