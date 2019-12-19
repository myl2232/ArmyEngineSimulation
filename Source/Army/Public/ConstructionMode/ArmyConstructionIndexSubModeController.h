/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRConstructionIndexSubModeController.h
 * @Description 索引图编辑子模式
 *
 * @Author HUAEN
 * @Date 2019年6月4日
 * @Version 1.0
 */

#pragma once

#include "ArmyDesignModeController.h"
#include "ArmyConstructionFunction.h"
#include "ArmyConstructionLayerManager.h"

class FArmyConstructionIndexSubModeController : public FArmyDesignModeController
{
public:
	enum EConstructionIndexSubModeOperation
	{
		HO_SYMBOLREGION
	};
    ~FArmyConstructionIndexSubModeController();

    //~ Begin FArmyDesignModeController Interface
    virtual void Init() override;
	virtual void InitOperations() override;
    virtual void BeginMode() override;
    virtual bool EndMode() override;
	virtual void EndOperation() override;
	virtual void Clear() override;
	virtual void Quit() override;
    virtual TSharedPtr<SWidget> MakeLeftPanelWidget() override;
    virtual TSharedPtr<SWidget> MakeRightPanelWidget() override;
    virtual TSharedPtr<SWidget> MakeToolBarWidget() override;
    virtual TSharedPtr<SWidget> MakeSettingBarWidget() override;
    //~ End FArmyDesignModeController Interface
	virtual void CollectAllLinesAndPoints(TArray< TSharedPtr<FArmyLine> >& InOutLines, TArray< TSharedPtr<FArmyPoint> >& InOutPoints, TArray<TSharedPtr<FArmyCircle>>& InOutCircles) override;
	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI);

	virtual void Draw(FViewport* InViewport, const FSceneView* View, FCanvas* SceneCanvas);

	virtual bool InputKey(FViewport* Viewport, FKey Key, EInputEvent Event) override;

	virtual void MouseMove(FViewport* Viewport, int32 X, int32 Y) override;
	virtual bool MouseDrag(FViewport* Viewport, FKey Key) override;

	virtual void Tick(float DeltaSeconds);

	/** 加载数据 */
	virtual void Load(TSharedPtr<FJsonObject> Data) override;

	/** 保存数据 */
	virtual bool Save(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter) override;

	virtual void Delete() override;

	void SetDataContainner(const TSharedPtr<FArmyLayer> InLayer);

	const TSharedPtr<FArmyLayer> GetDataContainner() const;

	void AddSymbolGroupData(const FGuid& InRoomId,const TSharedPtr<FIndexSymbolGroup> InSymbolGroup);

	void UpdateSymbol();

	const TMap<FGuid, TSharedPtr<FIndexSymbolGroup>>& GetSymbolGroupMap() const;
public:
	FInt32Delegate DelegateFloatButtonClicked;
private:
	void RefreshData();

	TSharedPtr<SWidget> MakeFloatWidget();

	FReply FloatButtonClicked(int32 InType);
private:
	TSharedPtr<class SWidget> FloatWidget;
	TSharedPtr<FArmyLayer> LocalDataLayer;
	TMap<FGuid, TSharedPtr<FIndexSymbolGroup>> SymbolGroupMap;

	//当前整体索引符号组
	TSharedPtr<FIndexSymbolGroup> CurrentSymbolGroup;

	//范围框
	TSharedPtr<FArmySymbolRectRegion> CurrentSymbolRegion;

	//当前选中的索引符号
	TSharedPtr<FArmySymbol> CurrentSymbol;

};