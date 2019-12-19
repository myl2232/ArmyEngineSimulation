/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 *  
 *
 * @File XRViewController.h
 * @Description 视图控制器基类
 *
 * @Author 欧石楠
 * @Date 2018年6月6日
 * @Version 1.0
 */

#pragma once

#include "ArmyTransactionMgr.h"
#include "ArmyScopedTransaction.h"
#include "SceneManagement.h"
#include "SArmySystemTitleBar.h"

typedef TSharedPtr<class FArmyViewController> FArmyViewControllerPtr;

class ARMYFRAME_API FArmyViewController
{
public:
    FArmyViewController()
        : CommandList(MakeShareable(new FUICommandList))
        , TransMgr(MakeShareable(new FArmyTransactionMgr))
    {}

    virtual ~FArmyViewController();

    /** 初始化 */
    virtual void Init();

	virtual bool Inited();

    // @zengy added
    virtual void Quit() {}
    
    /** 创建自定义标题栏内容 */
    virtual TSharedPtr<SWidget> MakeCustomTitleBarWidget() { return nullptr; }

    /** 创建窗口内容 */
    virtual TSharedPtr<SWidget> MakeContentWidget() = 0;

    virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) {}

    virtual void Draw(FViewport* InViewport, const FSceneView* View, FCanvas* SceneCanvas) {}

    virtual bool InputKey(FViewport* Viewport, FKey Key, EInputEvent Event) { return false; }

	virtual bool InputAxis (FViewport* Viewport,int32 ControllerId,FKey Key,float Delta,float DeltaTime,int32 NumSamples = 1,bool bGamepad = false) { return false; }

    /** 鼠标单击事件，所有的鼠标左键/右键/中键单击事件都在该方法中实现 */
    virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) {}

    virtual void Tick(float DeltaSeconds) {}

    virtual void MouseMove(FViewport* Viewport, int32 X, int32 Y) {}

	virtual bool MouseDrag(FViewport* Viewport, FKey Key) { return false; }

    virtual TSharedPtr<FUICommandList> GetCommandList() { return CommandList; }

public:
    TSharedPtr<SWidget> TitleBarWidget;

    TSharedPtr<SWidget> ContentWidget;

    /** 撤销重做队列管理类 */
    TSharedPtr<FArmyTransactionMgr> TransMgr;

    bool AlreadyInited = false;

protected:
    /** 视图控制器里管理的UICommand */
    TSharedPtr<FUICommandList> CommandList;
};

/**
 * 模态视图控制器
 */
class FArmyModalViewController : public FArmyViewController
{
public:
    //~ Begin FArmyViewController Interface
    virtual TSharedPtr<SWidget> MakeCustomTitleBarWidget() override { return nullptr; }
    //~ End FArmyViewController Interface
};

/** 模态对话框类型 */
enum EDialogType
{
    DT_ConfirmCancel,
    DT_Custom,
};

/** 新建一个视图控制器 */
template<class T>
static TSharedPtr<T> NewVC()
{
    TSharedPtr<T> NewVC = MakeShareable(new T());
    NewVC->Init();

    return NewVC;
}