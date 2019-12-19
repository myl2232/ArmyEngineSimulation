#pragma once
#include "../XRWHCAttrPanelMixin.h"

class FArmyWHCPlatformUI final : public TXRWHCAttrUIComponent<FArmyWHCPlatformUI>
{
public:
    FArmyWHCPlatformUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : TXRWHCAttrUIComponent<FArmyWHCPlatformUI>(InContainer)
    { Type = EWHCAttrUIType::EWAUT_Platform; }
    virtual TSharedPtr<SWidget> MakeWidget() override;
    void LoadListData();
    void SetAddOrReplace(bool bAddOrReplace_) { bAddOrReplace = bAddOrReplace_; }
    bool IsAdd() const { return bAddOrReplace; }

private:
    void Callback_ReplacingPlatform();
    void Callback_ReplacingPlatformItem(int32 InItemCode);
    FReply Callback_AddPlatform();
    void Callback_DeletePlatform();
    void LoadPlatformData();
    void FillPlatformList(int32 InStart, int32 InCount);

    void SetDownloadPlatformResState(bool bSucceeded, class SContentItem *UIItem, struct FMtlInfo *DataItem);

private:
    bool bAddOrReplace = false;
};