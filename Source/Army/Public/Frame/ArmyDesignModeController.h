/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRDesignModeController.h
 * @Description 设计模式控制器基类
 *
 * @Author 欧石楠
 * @Date 2018年6月8日
 * @Version 1.0
 */

#pragma once

#include "ArmyOperation.h"
#include "ArmySceneData.h"
#include "ArmyMouseCapture.h"
#include "ArmyRectSelect.h"
#include "ArmyToolsModule.h"
#include "ArmyViewController.h"
#include "ArmyCommonTypes.h"
#include "SArmyModelContentBrowser.h"

typedef TSharedPtr<class FArmyDesignModeController> FArmyDesignModeControllerPtr;

class FArmyDesignModeController : public FArmyViewController
{
public:
	virtual ~FArmyDesignModeController() {}

	//~ Begin FArmyViewController Interface
	virtual void Init() override;
	virtual TSharedPtr<SWidget> MakeCustomTitleBarWidget() override { return nullptr; }
	//~ End FArmyViewController Interface

    virtual const EModelType GetDesignModelType() { return EModelType::E_NoneModel; }

	/** 初始化操作类 */
	virtual void InitOperations() {}

	/** 模式开始 */
	virtual void BeginMode() { IsCurrentModel = true; };

	/** 模式结束 */
    virtual bool EndMode();

	/** 左面板内容 */
	virtual TSharedPtr<SWidget> MakeLeftPanelWidget() = 0;

	/** 右面板内容 */
	virtual TSharedPtr<SWidget> MakeRightPanelWidget() = 0;

	/** 工具栏内容 */
	virtual TSharedPtr<SWidget> MakeToolBarWidget() = 0;

	/** 设置栏内容 */
	virtual TSharedPtr<SWidget> MakeSettingBarWidget() = 0;

	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;

	virtual void Draw(FViewport* InViewport, const FSceneView* View, FCanvas* SceneCanvas) override;

	virtual bool InputKey(FViewport* Viewport, FKey Key, EInputEvent Event) override;

	virtual bool InputAxis(FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples = 1, bool bGamepad = false) override { return false; }

	virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void MouseMove(FViewport* Viewport, int32 X, int32 Y) override;

	virtual bool MouseDrag(FViewport* Viewport, FKey Key) override;

	/** 设置当前操作 */
	virtual void SetOperation(uint8 Operation, XRArgument InArg = XRArgument());

	virtual void EndOperation();

	/** 清空操作 */
	virtual void Clear() {}

	/** @欧石楠 删除操作 */
	virtual void Delete() {}

	/** 加载数据 */
	virtual void Load(TSharedPtr<FJsonObject> Data) {}

	/** 保存数据 */
	virtual bool Save(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter) { return false; }

	virtual void CollectAllLinesAndPoints(TArray< TSharedPtr<FArmyLine> >& InOutLines, TArray< TSharedPtr<FArmyPoint> >& InOutPoints, TArray<TSharedPtr<FArmyCircle>>& InOutCircles) {};

	const TSharedPtr<FArmyOperation> GetCurrentOperation() const;

	virtual void ThreeTo2DGanged(TSharedPtr<FContentItemSpace::FComponentRes> InRes, AActor* InActor, TSharedPtr<FArmyObject> Object = nullptr) {};

    /** 隐藏设计操作面板，锁定和解锁模式时使用 */
    void HideDesignPanel(bool bHidden);

	uint32 GetCurrentOperationType() const;

	bool IsCurrentOperration(uint8 OperationKey){return CurrentOperation == OperationMap.FindRef(OperationKey);}
protected:
    bool ShouldTick(); 

private:
	//~ Begin FArmyViewController Interface
	virtual TSharedPtr<SWidget> MakeContentWidget() override;
	//~ End FArmyViewController Interface

	/** 设置各个面板里的内容 */
	void SetPanelContent(TSharedPtr<SBox> Panel, TSharedPtr<SWidget> Content);

protected:
	TMap< uint8, TSharedPtr<FArmyOperation> > OperationMap;

	TArray<FArmyRectSelect::ObjectInfo> AllCanHoverObjects;

	//@郭子阳
	/** 上一个操作 */
	TSharedPtr<FArmyOperation> FormerOperation;

	/** 当前操作类 */
	TSharedPtr<FArmyOperation> CurrentOperation;

    TSharedPtr<class SArmyDesignFrame> DesignFrame;

	/** 是否是当前模式*/
	bool IsCurrentModel = false;

    FVector2D ViewportSize;

    FVector2D LastViewportMousePos;

    FVector2D ViewportMousePos;
};