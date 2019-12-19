#pragma once
#include "SArmyWHCModeAttrPanel.h"

class FArmyWHCModeAccAttr : public FArmyWHCModeAttrPanel
{
public:
    FArmyWHCModeAccAttr(TSharedRef<SArmyWHCModeAttrPanelContainer> InContainer);
    virtual void TickPanel() override;
    virtual void Initialize() override;
    virtual bool TryDisplayAttrPanel(class AActor *InActor) override;
    virtual void LoadMoreReplacingData() override;
    virtual TSharedPtr<SWidget> RebuildAttrPanel() override;

protected:
    virtual void TryDisplayAttrPanelInternal(class AXRElecDeviceActor *InActor);
    virtual bool CheckCanShowAccAttr(class AXRElecDeviceActor *InActor) const { return true; }

private:
    class FArmyDetailNode* AddDetailNode(FName Name, const FString &DisplayName);
    void RemoveDetailNode(FName Name);
    TSharedPtr<SEditableText> AddEditableTextNode(class FArmyDetailNode *ParentNode, FName Name, const FString &DisplayName, 
        FOnIsTypedCharValid IsTypedCharValid, 
        FOnTextCommitted TextCommitted);

    bool Callback_IsNumericWithMinus(const TCHAR c);
    bool Callback_IsNumericWithoutMinus(const TCHAR c);
    // 修改附属件的左右位置
    void Callback_LeftRightTextCommitted(const FText &InText, ETextCommit::Type InType);
    // 修改和台面的距离
    void Callback_AboveTableCommitted(const FText &InText, ETextCommit::Type InType);

    bool IsTopCategory(EMetalsType InMetalsType) const;

protected:
    TSharedPtr<class FArmyDetailBuilder> AccPanel;
};

