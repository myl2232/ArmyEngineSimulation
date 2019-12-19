#pragma once
#include "Widgets/SCompoundWidget.h"

class SArmyWHCModeListPanel : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyWHCModeListPanel)
        {}
        SLATE_EVENT(FOnClicked, OnPanelClosed)
    SLATE_END_ARGS()

    void Construct(const FArguments &InArgs);
     
    void SetCurrentState(const FString &InNewState);
    const FString& GetCurrentState() const { return CurrentState; }

    void SetListController(TSharedRef<SWidget> InController);
    void ClearListController();

    void AddItem(const TSharedPtr<class SContentItem> &InNewItem);
	const TArray<TSharedPtr<class SContentItem>>& GetItems() const;
	void ClearItems();

    void SetSelectedItem(int32 InIndex);
    class SContentItem* GetSelectedItem() const;
    int32 GetSelectedItemIndex() const;

    TSharedPtr<class SScrollWrapBox> Container;

    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    
private:
    TArray<TSharedPtr<class SContentItem>> ContentItemList;
    TSharedPtr<class STextBlock> PanelTitle;
    SVerticalBox::FSlot *ListControllerSlot;
    SContentItem *LastSelected;
    FString CurrentState;
};