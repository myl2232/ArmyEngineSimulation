/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRHomeModeController.h
 * @Description 户型模式视图控制器
 *
 * @Author 欧石楠
 * @Date 2018年6月7日
 * @Version 1.0
 */

#pragma once

#include "SArmyObjectTreeItem.h"
#include "ArmyDesignModeController.h"

class FArmyHomeModeController : public FArmyDesignModeController, public TSharedFromThis<FArmyHomeModeController>
{
public:
    ~FArmyHomeModeController();

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
    virtual void Clear() override;
    virtual void Load(TSharedPtr<FJsonObject> Data) override;
    virtual bool Save(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter) override;
	virtual void Delete() override;
    //~ End FArmyDesignModeController Interface

	void OnObjectOperation(const XRArgument& InArg, FObjectPtr InObj,bool bTransaction=false);

	void OnHomeModeModifyOperation();

	//画原始内墙
	void OnInternalWallCommand();

	//画开阳台
	void OnBalconyPunchCommand();
	//@ 画承重墙墙体
	void OnBearingWallCommand();

	/**@欧石楠 检测房间是否有命名*/
	bool CheckRoomName();

	/**@欧石楠 设置锁定/解锁户型*/
	void SetLockHome(bool bLocked);

private:
	/** @欧石楠 ToolBar Function*/
	//保存
	void OnSaveCommand();

	//撤销
	void OnUndoCommand();

	//重做
	void OnRedoCommand();

	//清空
	void OnEmptyCommand();

    /** 导入 */
    TSharedRef<SWidget> OnImportCommand();
    void OnImportFacsimile();
    void OnSetFacsimileScale();
    bool CanSetFacsimileScale();
    bool bFacsimileImported = false;

	//偏移
	void OnOffsetCommand();

	//生成外墙
	void OnOutWallCommand();
	//删除外墙
	void OnDeleteOutWallCommand();
	void OnDeleteOutWallExcute();

	void OnPassCommand();
	void OnSecurityDoorCommand();
	void OnDoorWayCommand() {}
	void OnWindowCommand();
	void OnFloorWindowCommand();
	void OnBayWindowCommand();
    void OnCornerBayWindowCommand();
    void OnTrapezoidBayWindowCommand();
	void OnArcWindowCommand() {}

	/** @欧石楠 空调孔操作*/
	void OnAirLouverCommand();

	void OnComponentCommand(XRArgument InArg);

	//点位
	void OnPointLocationCommand(XRArgument InArg);

	/** @欧石楠 选中操作 */
	void OnSelectionChanged(TArray< TSharedPtr<FArmyObject> >& ObjArray);
	
	bool CheckCanDeleteOutWall() { return FArmySceneData::Get()->GetHasOutWall(); }
	bool CheckCanCreateOutWall() { return !FArmySceneData::Get()->GetHasOutWall(); }

	void OnDeleteExcute();

	/**@欧石楠 当点击确认生成外墙时*/
	void OnConfirmGenerateOutWallClicked();

	/**@欧石楠 不命名直接切换模式*/
	void OnNotNameSpace();

	/**@欧石楠 不切换模式，去命名*/
	void OnNameSpace();

	/**@欧石楠 记录是否要直接切换模式或者去命名*/
	bool bSwitchMode = false;

	/**
	 * @ 设置对象的四轴标尺是否可见
	 * @param obj - TSharedPtr<FArmyObject> - 所要设置的对象
	 * @param bShow - bool - true表示可见，否则不可见
	 * @return void - 
	 */
	void ShowAxisRuler(TSharedPtr<FArmyObject> obj, bool bShow, bool bAutoAttach);

	/**
	* 预计算四轴标尺修改是否越界，此操作不修改选择对象数据
	* @param SelectedObj - TSharedPtr<FArmyObject> - 选择的对象
	* @param trans - FTransform - 变换矩阵
	* @param Flag - int32 -
	* @return bool - true表示操作越界，否则不越界
	*/
	bool PreCalTransformIllegalCommon(TSharedPtr<FArmyObject> SelectedObj, FTransform trans, int32 Flag);

	/**@欧石楠 生成解锁户型widget*/
	TSharedPtr<SWidget> CreateLockWidget();

	/**@欧石楠 点击解锁户型*/
	FReply OnUnlockClicked();

	/**@欧石楠 确定解锁户型*/
	void OnConfirmUnlockHome();

private:
	bool EnableMouseCapture = false;

	/**@欧石楠 是否锁定原始户型*/
	bool bLockedHome = false;
	/**@欧石楠 是否锁定原始户型的widget*/
	TSharedPtr<SWidget> LockWidget;

	/**@欧石楠 处于删除房间时*/
	bool bDeletingRoom = false;
	/**@欧石楠 四轴标尺*/
	TSharedPtr<class FArmyAxisRuler>AxisRuler;

	TSharedPtr<class SArmyObjectOutliner> ObjectOutLiner;
	TMap<FGuid, SArmyObjectTreeItemPtr> TreeItemMap;

	TMap<FName, TWeakPtr<class FArmyObjLayer>> HomeDataLayers;

	bool bRebuildBspWall = true;

    TSharedPtr<SWidgetSwitcher> ToolBarSwitcher;

	TSharedPtr<class SArmyWallSetting> WallSettingWidget;

	TSharedPtr<class SArmyFacsimileSetting> FacsimileSettingWidget;

	TSharedPtr<class FArmyHomeModeDetail> DetailWidget;

	TSharedPtr<class SArmyGenerateOutWall> GenerateOutWallWidget;

	TSharedPtr<class FArmyHardwareOperation> HardWareOperation;

    TSharedPtr<class FArmyComponentOperation> ComponentOperation;

	TSharedPtr<class FArmyRoomOperation> RoomOperation;
};