#pragma once
#include "ArmyDesignModeController.h"
#include "ArmyWHCabinetAutoMgr.h"

class FArmyWHCModeController : public FArmyDesignModeController
{
public:
    FArmyWHCModeController();
    virtual ~FArmyWHCModeController();

    virtual const EModelType GetDesignModelType() override;
    virtual void Init() override;
    virtual void Quit() override;
    virtual void BeginMode() override;
    virtual bool EndMode() override;
    virtual TSharedPtr<SWidget> MakeLeftPanelWidget() override;
    virtual TSharedPtr<SWidget> MakeRightPanelWidget() override;
    virtual TSharedPtr<SWidget> MakeToolBarWidget() override;
    virtual TSharedPtr<SWidget> MakeSettingBarWidget() override;
    virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
    virtual bool InputKey(FViewport* Viewport, FKey Key, EInputEvent Event) override;
    virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void MouseMove(FViewport* Viewport, int32 X, int32 Y) override;
    virtual bool MouseDrag(FViewport* Viewport, FKey Key) override;
	virtual void Load(TSharedPtr<FJsonObject> Data) override;
	virtual bool Save(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter) override;
    virtual void Clear() override;
    virtual void Delete() override;

private:
    void Callback_SaveCommand();
    void Callback_DeleteCommand();

    TSharedRef<SWidget> Callback_HideCommandForCupboard();
    TSharedRef<SWidget> Callback_HideCommandForWardrobe();
    TSharedRef<SWidget> Callback_HideCommandForOther();
    void Callback_HideCabinetDoorCommand();
    void Callback_HideOnGroundCabCommand();
    void Callback_HideHangCabCommand();
    void Callback_HideFurniture();
    void Callback_HideHydropower();

    void Callback_StyleCommand();

    TSharedRef<SWidget> Callback_RulerCommand();
    void Callback_HideCabinetRulerCommand();
    void Callback_HideEnvRulerCommand();

    TSharedRef<SWidget> Callback_GeneratingCommand();
    void Callback_GeneratingPlatformCommand();
    void Callback_GeneratingToeCommand();
    void Callback_GeneratingTopBlockerCommand();
    void Callback_GeneratingBottomBlockerCommand();

    TSharedRef<SWidget> Callback_ClearSceneCommand();
    void Callback_ClearCabinetCommand();
    void Callback_ClearWardrobeCommand();
    void Callback_ClearOtherCommand();
    void Callback_ClearAllCommand();

    void Callback_TopdownViewCommand();
    void Callback_RegularViewCommand();

    void Callback_ClickedTopCategory(int32 Key, int32 Value);
    void Callback_ClickedCategory(int32 Key, int32 Value);
    void Callback_ItemListLoadMore();

    void Callback_ItemClicked(int32 InItemCode);
    void Callback_ItemDetailInfoHovered(int32 InItemCode);
    void Callback_ItemDetailInfoUnhovered(int32 InItemCode);
	void Callback_Search(const FString &InSearchText);

    void Callback_CabJsonDownloadFinished(bool bSucceeded, FString InJsonFileCachePath, int32 Id);
    void Callback_CabResourcesDownloadFinished(bool bSucceeded, FString InJsonFileCachePath, int32 InShapeId);

	void Callback_HideSliderDoorCommand();
	void Callback_HideDrawerFaceDoorCommand();

public:
	/** Auto Design About */
	FReply Callback_StartAutoDesign();

	//橱柜选型页
	FReply Callback_CloseCabinetTypeChoicePage();              //关闭橱柜选型页面
	FReply Callback_WhenSelectCabinetTypeDone();               //橱柜选择完毕，点击下一步  
	FReply Callback_SelectedCabinetType();                     //选择某个类型的厨具
	FReply Callback_SelectedCabinetItemType();                 //点击某个厨具类型下的‘款式选择’
	FReply Callback_OnHoverInformationCabinetType();
	
	//厨房选型页
	FReply Callback_OnSelectedLType();
	FReply Callback_OnSelectedHorizontalLineType();
	FReply Callback_OnSelectedUType();
	FReply Callback_OnSelectedDoubleHorizontalLineType();
	FReply Callback_SelectedKitchenType();
	FReply Callback_ReturnToCabinetTypeChoicePage();
	FReply Callback_WhenSelectKitchenTypeDone();

	//方案展示页
	FReply Callback_CabinetBeSelectedClicked();                //已选商品页面
	FReply Callback_ApplaySolution();
	FReply Callback_ReturnToKitchenTypeChoicePage();

	//主页面(包含厨房选型和方案展示两个子页面)
	FReply Callback_CloseAutoDesignMainPage();
	//面包屑

	//橱柜项选择页面
	FReply Callback_CloseCabinetItemChoicePage();
	FReply Callback_SelectedCabinetItem();

	//厨房列表页面
	FReply Callback_OnButtonKitchenItemClicked();
	FReply Callback_OnKitchenChoiceDone();
	FReply Callback_OnKitchenChoiceCancel();
	

private:
    void QueryCabinetList(int32 InClassify, int32 InType, const FString &InSearchText = TEXT(""));
    void FillCabinetList(int32 InCategory, int32 StartIndex, int32 InCount);
    int32 GetTopCategory(int32 InType) const;
    class SContentItem* GetListItem(int32 Id) const;

    TSharedPtr<SWidget> MakeToolBarWidgetForCupboardCabs();

    void ClearCabinet();
    void ClearWardrobe();
    void ClearOther();

private:
    // 褰撳墠姝ｅ湪鏄剧ず鐨勬煖瀛愬垎绫诲湪鐣岄潰涓婄殑绱㈠紩
    int32 CurrentDisplayingCategoryIndex;
    // 褰撳墠姝ｅ湪浣跨敤鐨勬悳绱㈠叧閿瓧
    FString CurrentSearchKeyword;

    FVector2D LastMousePosition;

    TSharedPtr<class SArmyWHCModeContentBrowser> ContentBrowser;
    TSharedPtr<class SArmyDetailInfoWidget> ContentItemDetailInfoWidget;
    TSharedPtr<class SArmyWHCModeAttrPanelContainer> ShapeDetailInfo;

	TSharedPtr<class FArmyWHCModeCabinetOperation> CabinetOperation;
	TSharedPtr<class FArmyWHCabinetMgr> mCablinMgr;
};