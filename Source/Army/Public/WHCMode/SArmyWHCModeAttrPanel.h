#pragma once
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SToolTip.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableText.h"
#include "Types/ISlateMetaData.h"
#include "Views/SListView.h"
#include "Styling/SlateBrush.h"
#include "Styling/CoreStyle.h"
#include "GameFramework/Actor.h"
#include "Dom/JsonObject.h"
#include "ArmyTypes.h"
#include "ArmyWHCAttrPanelMixin.h"

enum class EWHCModeAttrType
{
    EWHCAttr_Unknown = -1,
    EWHCAttr_Cab,
    EWHCAttr_CabMtl,
    EWHCAttr_DoorType,
    EWHCAttr_DoorMtl,
    EWHCAttr_DoorHandle,
    EWHCAttr_DrawerType,
    EWHCAttr_DrawerMtl,
    EWHCAttr_PlatformMtl,
    EWHCAttr_PlatformFront,
    EWHCAttr_PlatformBack,
    EWHCAttr_ToeType,
    EWHCAttr_ToeMtl,
    EWHCAttr_TopBlockerType,
    EWHCAttr_TopBlockerMtl,
    EWHCAttr_Acc,
    EWHCAttr_CabAcc
};

enum class EHttpErrorReason
{
    EReason_HttpCodeError,
    EReason_NoValidData
};

struct FWHCModeAttrTypeInfo : public ISlateMetaData
{
    EWHCModeAttrType Type;
    int32 Id;
    FWHCModeAttrTypeInfo(EWHCModeAttrType InType, int32 InId) : Type(InType), Id(InId) {}
    static FName GetTypeId() { return FName("WHCModeAttrTypeInfo"); }
protected:
    virtual bool IsOfTypeImpl(const FName& InType) const override
    {
        return InType == "WHCModeAttrTypeInfo";
    }
};

class SArmyWHCModeNormalComboBox : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyWHCModeNormalComboBox)
        {}
        SLATE_ARGUMENT(TArray<TSharedPtr<FString>>, OptionSource)
        SLATE_ARGUMENT(FString, Value)
        SLATE_EVENT(FStringDelegate, OnValueChanged)
    SLATE_END_ARGS()

    void Construct(const FArguments &InArgs);
    void SetOptionSource(const TArray<TSharedPtr<FString>> &InOptionSource);
    void SetValueAttr(const TAttribute<FText> &InText);
    // TODO: Remove this function
    void SetValue(const FString &InText);
    FString GetValue() const;

    TSharedRef<SWidget> OnGenerateWidget(TSharedPtr<FString> InItem);
    void OnSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);

private:
    TSharedPtr<SComboBox<TSharedPtr<FString>>> ComboBox;
    TSharedPtr<STextBlock> Value;
    TArray<TSharedPtr<FString>> OptionsSource;
    FStringDelegate OnValueChanged;
};

class SArmyWHCModeModificationRange : public SCompoundWidget
{
public:
    enum EModificationRange
    {
        MR_Single,
        MR_SingleCab,
        MR_SameType,
        MR_All
    };

    SLATE_BEGIN_ARGS(SArmyWHCModeModificationRange)
        : _bHasSingleRadio(true)
        , _bHasCabRadio(true)
        , _bHasSameTypeRadio(true)
        , _bHasAllRadio(true)
        {}
        SLATE_ARGUMENT(bool, bHasSingleRadio)
        SLATE_ARGUMENT(bool, bHasCabRadio)
        SLATE_ARGUMENT(bool, bHasSameTypeRadio)
        SLATE_ARGUMENT(bool, bHasAllRadio)
    SLATE_END_ARGS()

    void Construct(const FArguments &InArgs);
    void Reset(EModificationRange InRange);
    EModificationRange GetRange() const { return MRange; }

private:
    void SwitchRange(EModificationRange InNewRange);

private:
    TSharedPtr<SCheckBox> SingleRadio;
    TSharedPtr<SCheckBox> SingleCabRadio;
    TSharedPtr<SCheckBox> SameTypeRadio;
    TSharedPtr<SCheckBox> AllRadio;

    EModificationRange MRange;
};

class SArmyWHCModeReplacingWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyWHCModeReplacingWidget)
        : _ButtonImage(nullptr)
        {}
        SLATE_ARGUMENT(const FSlateBrush*, ButtonImage)
        SLATE_EVENT(FSimpleDelegate, OnWholeWidgetClicked)
        SLATE_EVENT(FSimpleDelegate, OnButtonClicked)
    SLATE_END_ARGS()

    void Construct(const FArguments &InArgs);

    void SetReplacingItemThumbnail(TAttribute<const FSlateBrush *> InThumbnail);
    void SetReplacingItemText(const TAttribute<FText> &InText);
    void SetReplacingItemText(const FText &InText);
    void SetReplacingItemToolTip(const FText &InText);
    void SetButtonImage(const FSlateBrush *InButtonImage);

    FSimpleDelegate& GetWholeWidgetClicked();
    FSimpleDelegate& GetButtonClicked();

private:
    TSharedPtr<SImage> ThumbnailImage;
    TSharedPtr<STextBlock> Text;
    TSharedPtr<SImage> ButtonImage;

    TSharedPtr<SBox> Container;
    TSharedPtr<SToolTip> ToolTip;

    FSimpleDelegate OnWholeWidgetClicked;
    FSimpleDelegate OnButtonClicked;
};

class SArmyWHCModeReplacingWidgetWithOverlayMenu : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyWHCModeReplacingWidgetWithOverlayMenu)
        {}
        SLATE_ARGUMENT(TSharedPtr<SOverlay>, OverlayWidgetForMenu)
        SLATE_EVENT(FStringDelegate, OnMenuItemSelectionChanged)
    SLATE_END_ARGS()

    void Construct(const FArguments &InArgs);

    void SetReplacingItemThumbnail(TAttribute<const FSlateBrush *> InThumbnail);
    void SetReplacingItemText(const FText &InText);
    void SetReplacingItemToolTip(const FText &InText);

private:
    TSharedPtr<SImage> ThumbnailImage;
    TSharedPtr<STextBlock> Text;
    
    TWeakPtr<SOverlay> OverlayRef;
    SOverlay::FOverlaySlot *MenuSlot = nullptr;
    TSharedPtr<SBox> Menu;
    TArray<TSharedPtr<FString>> MenuOptions;

    TSharedPtr<SBox> Container;
    TSharedPtr<SToolTip> ToolTip;

    FStringDelegate OnMenuItemSelectionChanged;
};

class SArmyWHCModeAddDelWidget : public SCompoundWidget
{
public:
    DECLARE_DELEGATE_RetVal(bool, FOnAdd)
    SLATE_BEGIN_ARGS(SArmyWHCModeAddDelWidget)
        {}
        SLATE_EVENT(FOnAdd, OnAdd)
        SLATE_EVENT(FSimpleDelegate, OnDel)
    SLATE_END_ARGS()

    void Construct(const FArguments &InArgs);

    void SetAddOrDel(bool IsAdded);
    void SetDisplayTextAttr(const TAttribute<FText> &InText);
    // TODO: Remove this function
    void SetDisplayText(const FText &InText);
    void SetDisplayToolTip(const FText &InText);

private:
    FReply Callback_Add();
    FReply Callback_Del();

private:
    TSharedPtr<SBox> AddButtonContainer;
    TSharedPtr<SBox> DelButtonContainer;
    TSharedPtr<STextBlock> DisplayText;

    FOnAdd OnAdd;
    FSimpleDelegate OnDel;
};

class FArmyWHCModeAttrPanel
{
public:
    friend class SArmyWHCModeAttrPanelContainer;

    FArmyWHCModeAttrPanel(TSharedRef<class SArmyWHCModeAttrPanelContainer> InContainer);
    virtual ~FArmyWHCModeAttrPanel() {}

    virtual void TickPanel() {};
    virtual void Initialize() {};
    virtual bool TryDisplayAttrPanel(AActor *InActor) { (void)InActor; return false; };
    virtual void LoadMoreReplacingData() {};
    virtual TSharedPtr<SWidget> RebuildAttrPanel() { return nullptr; }

protected:
    // 显示加载信息
    void ShowLoadingThrobber(const FString &InStr);
    // 隐藏加载信息
    void HideLoadingThrobber();
    // 是否为有效输入数字
    bool IsValidNumberText(const FString &InStr) const;
    // 更新下载进度
    void UpdateProgress(const TSharedPtr<class FDownloadFileSet> &InTask, class SContentItem *InContentItem);

    TWeakPtr<class SArmyWHCModeAttrPanelContainer> Container;
    TArray<TSharedPtr<IArmyWHCAttrUIComponent>> ComponentArr;
};

class FArmyWHCModeCabinetOperation;
class SArmyWHCModeAttrPanelContainer : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyWHCModeAttrPanelContainer)
        {}
        SLATE_ARGUMENT(FArmyWHCModeCabinetOperation*, CabinetOperation)
    SLATE_END_ARGS()

    void Construct(const FArguments &InArgs);
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    void Initialize();
    void BeginMode();
    void EndMode();

    void Callback_WHCItemSelected(class AActor *InActor);
    void Callback_WHCItemDeleted();
    template<class Type>
    bool IsWHCItemSelected() const { return LastWHCItemSelected && LastWHCItemSelected->IsA<Type>(); }

    void SetAttrPanel(TSharedRef<SWidget> InPanel);
    void CloseAttrPanel();
    
    void SetReplacingPanel(TSharedRef<SWidget> InPanel);
    FReply CloseReplacingPanel();
    bool IsClickedSameReplacingItem(int32 InIndex) const;

    void Callback_LoadMoreReplacingData();

    void ShowInteractiveMessage(bool bShow);

    void NotifyAttrPanelNeedRebuild();


    // FArmyWHCModeAttrPanel包装函数
    // 清空可替换信息
    void ClearReplacingList();
    // 是否是有效的索引
    bool CheckIsValidReplacingIndex(int32 InIndex) const;
    // 获取当前要请求的分页
    int32 GetPageIndexToLoad() const;
    // 是否可以继续加载
    bool CheckNoMoreData() const;
    // 获取列表项
    class SContentItem* GetListItem(int32 InIndex) const;
    // 根据项ID和类型获取列表项
    class SContentItem* GetListItemMatchingType(int32 InId, EWHCModeAttrType InMatchingType) const;
    // 设置当前状态
    void SetCurrentState(const FString &InState);
    // 获取当前状态
    const FString& GetCurrentState() const;
    // 是否处于某个状态
    bool IsCurrentState(const FString &InTestState) const;
    // 进行Http请求
    void RunHttpRequest(const FString &InUrl, 
        TBaseDelegate<void, const TSharedPtr<FJsonObject>&> &&ResponseCallback,
        TBaseDelegate<void, EHttpErrorReason> &&ErrorCallback = TBaseDelegate<void, EHttpErrorReason>());
    void RunHttpRequest(const FString &InUrl,
        const TBaseDelegate<void, const TSharedPtr<FJsonObject>&> &ResponseCallback,
        const TBaseDelegate<void, EHttpErrorReason> &ErrorCallback = TBaseDelegate<void, EHttpErrorReason>());
    // 设置总分页数
    void SetReplacingTotalPage(int32 InTotalPage);
    // 添加可替换项Id
    void AddReplacingId(int32 InId);
    // 获取所有的可替换Id
    const TArray<int32>& GetReplacingIds() const;

    TSharedPtr<SOverlay> ContentOverlay;
    TSharedPtr<SBorder> ContentSlot;
    SOverlay::FOverlaySlot *ReplacingSlot;

    TSharedPtr<class SArmyWHCModeListPanel> ListPanel;

    AActor *LastWHCItemSelected;
    FArmyWHCModeCabinetOperation *CabinetOperationRef;


private:
    TArray<TSharedPtr<FArmyWHCModeAttrPanel>> AttrPanels;
    TWeakPtr<FArmyWHCModeAttrPanel> CurrentAttrPanel;

    struct FReplacingInfo {
        int32 TotalDataPage;
        TArray<int32> ReplacingIds;
    } ReplacingInfo;
};