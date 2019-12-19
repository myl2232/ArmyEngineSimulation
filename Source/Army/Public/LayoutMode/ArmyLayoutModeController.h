/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRHomeModeController.h
 * @Description 布局模式视图控制器
 *
 * @Author 欧石楠
 * @Date 2018年6月7日
 * @Version 1.0
 */

#pragma once

#include "ArmyDesignModeController.h"

class FArmyLayoutModeController : public FArmyDesignModeController
{
public:
    ~FArmyLayoutModeController();

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
    virtual TSharedPtr<SWidget> MakeSettingBarWidget() override { return SNullWidget::NullWidget; }
	virtual void CollectAllLinesAndPoints(TArray< TSharedPtr<FArmyLine> >& InOutLines, TArray< TSharedPtr<FArmyPoint> >& InOutPoints, TArray<TSharedPtr<FArmyCircle>>& InOutCircles) override;
	virtual void Draw(FViewport* InViewport,const FSceneView* InView, FCanvas* SceneCanvas) override;
	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI);
	virtual bool InputKey(FViewport* Viewport, FKey Key, EInputEvent Event) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void MouseMove(FViewport* Viewport, int32 X, int32 Y) override;
    virtual void Clear() override;
    virtual void Load(TSharedPtr<FJsonObject> Data) override;
    virtual bool Save(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter) override;
	virtual void Delete() override;
    //~ End FArmyDesignModeController Interface

    void OnObjectOperation(const XRArgument& InArg, FObjectPtr InObj, bool bTransaction = false);

	/**@欧石楠将导入临摹图和划线挪到public，用于首次创建方案时，选择不同方式直接激活operation*/
	/** 画新建墙体 */
	void OnAddWallCommand();

	/** 画拆除墙体 */
	void OnModifyWallCommand();

	/** 包立管 */
	void OnPackPipeCommand();

	void OnModifyModeModifyOperation();

	/** @纪仁泽 进行区域分割 */
	void OnSplitRegion();

private:
	/** @欧石楠 ToolBar Function*/
	//保存
	void OnSaveCommand();

//	//撤销
//	void OnUndoCommand();
//
//	//重做
//	void OnRedoCommand();

	//清空
	void OnEmptyCommand();
		
	void OnDoorCommand();

	void OnSlidingDoorCommand();

	void OnComponentCommand(XRArgument InArg);

	/**@欧石楠 新开垭口*/
	void OnNewPassCommand();

	/**@欧石楠选中操作*/
	void OnSelectionChanged(TArray< TSharedPtr<FArmyObject> >& ObjArray);

	void OnDeleteExcute();

    TSharedRef<SWidget> OnDismantleStateCommand();

    bool IsDisplayDismantle() { return FArmySceneData::Get()->bIsDisplayDismantle; }

    bool IsDisplayAfterDismantle() { return !FArmySceneData::Get()->bIsDisplayDismantle; }

    /** 显示拆改中的户型状态 */
    void OnDisplayDismantle();

    /** 显示拆改后的户型状态 */
    void OnDisplayAfterDismantle();

	void OnClear();

//	/** 生成所有的BSP*/
//	void GenerateBSP();

//	void SplitRooms();
//
//	void MergeRooms();
//
//	/**
//	 * @ 设置对象的四轴标尺是否可见
//	 * @param obj - TSharedPtr<FArmyObject> - 所要设置的对象
//	 * @param bShow - bool - true表示可见，否则不可见
//	 * @return void - 
//	 */
//	void ShowAxisRuler(TSharedPtr<FArmyObject> obj, bool bShow, bool bAutoAttach);

	// 初始化区域中的分割线
	void InitSplitLineInRegion();
private:
//	bool EnableMouseCapture = false;
//
//	/**@欧石楠 处于删除房间时*/
//	bool bDeletingRoom = false;

	/**@欧石楠 四轴标尺*/
	TSharedPtr<class FArmyAxisRuler> AxisRuler;

//	TSharedPtr<class SArmyObjectOutliner> ObjectOutLiner;

    TMap< FName, TWeakPtr<class FArmyObjLayer> > HomeDataLayers_Modify;
	TMap< FName, TWeakPtr<class FArmyObjLayer> > HomeDataLayers_Layout;

    /** @欧石楠 底图 */
    TWeakPtr<FArmyObject> ReferenceImage = nullptr;

	TSharedPtr<class FArmyLayoutModeDetail> DetailWidget;

    TSharedPtr<class FArmyHardwareOperation> HardWareOperation;

	TSharedPtr<class FArmyComponentOperation> ComponentOperation;

	//@ 保存可hover对象，用于拆改后
	TArray<FArmyRectSelect::ObjectInfo> AllCanHoverObjectsDismantle;

};