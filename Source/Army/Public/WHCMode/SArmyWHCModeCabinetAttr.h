#pragma once
#include "SArmyWHCModeAttrPanel.h"
#include "Widgets/Input/SEditableText.h"

class FArmyWHCModeCabinetAttr : public FArmyWHCModeAttrPanel
{
public:
    FArmyWHCModeCabinetAttr(TSharedRef<SArmyWHCModeAttrPanelContainer> InContainer);
    virtual void TickPanel() override;
    virtual void Initialize() override;
    virtual bool TryDisplayAttrPanel(class AActor *InActor) override;
    virtual void LoadMoreReplacingData() override;
    virtual TSharedPtr<SWidget> RebuildAttrPanel() override;

protected:
    virtual void TryDisplayAttrPanelInternal(struct FShapeInRoom *InShapeInRoom);
    virtual void RebuildAttrPanelInternal(struct FShapeInRoom *InShapeInRoom);
    virtual bool CheckCanShowCabinetAttr(struct FShapeInRoom *InShapeInRoom) const;

    void MakeDimensionUI();
    void MakeDecorationBoardUI(struct FShapeInRoom *InShapeInRoom);
	
protected:
	void Callback_UpValueCommitted(const FText& InText, const ETextCommit::Type InTextAction);
	void Callback_FrontValueCommitted(const FText& InText, const ETextCommit::Type InTextAction);
	void Callback_RightValueCommitted(const FText& InText, const ETextCommit::Type InTextAction);
    
    bool CheckIfChangeAllCabHeights(struct FShapeInRoom *InShapeInRoom) const;

    TSharedPtr<SEditableText> AddEditableTextNode(class FArmyDetailNode *ParentNode, FName Name, const FString &DisplayName, 
        FOnIsTypedCharValid IsTypedCharValid, 
        FOnTextCommitted TextCommitted);

    bool Callback_IsNumericWithMinus(const TCHAR c);
    bool Callback_IsNumericWithoutMinus(const TCHAR c);

protected:
    TSharedPtr<class FArmyDetailBuilder> CabinetDetailPanel;

    TSharedPtr<SEditableText> FrontBoardCabDoorWidthEditable;
	TSharedPtr<SEditableText> LeftRightEditable;
	TSharedPtr<SEditableText> UpDownEditable;
	TSharedPtr<SEditableText> FrontBackEditable;
};

class FArmyWHCModeWithFrontBoardCabinetAttr final : public FArmyWHCModeCabinetAttr
{
public:
    FArmyWHCModeWithFrontBoardCabinetAttr(TSharedRef<SArmyWHCModeAttrPanelContainer> InContainer)
    : FArmyWHCModeCabinetAttr(InContainer) {}

    virtual void Initialize() override;

protected:
    virtual void TryDisplayAttrPanelInternal(struct FShapeInRoom *InShapeInRoom) override;
    virtual void RebuildAttrPanelInternal(struct FShapeInRoom *InShapeInRoom) override;
    virtual bool CheckCanShowCabinetAttr(struct FShapeInRoom *InShapeInRoom) const override;

private:
    void Callback_DoorOpenDirChanged(const FString &InText);
    void Callback_DoorSheetWidthValueCommitted(const FText& InText, const ETextCommit::Type InTextAction);
    void MakeFrontBoardCabUI(struct FShapeInRoom *InShapeInRoom);

private:
    TSharedPtr<SArmyWHCModeNormalComboBox> FrontBoardCabOpenDoorType;
};