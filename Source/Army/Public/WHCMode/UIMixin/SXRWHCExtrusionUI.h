#pragma once
#include "../XRWHCAttrPanelMixin.h"

class FArmyWHCExtrusionUI final : public TXRWHCAttrUIComponent<FArmyWHCExtrusionUI>
{
public:
    FArmyWHCExtrusionUI(class SArmyWHCModeAttrPanelContainer *InContainer, int32 ExtrusionType_)
    : TXRWHCAttrUIComponent<FArmyWHCExtrusionUI>(InContainer)
    , ExtrusionType(ExtrusionType_)
    { Type = EWHCAttrUIType::EWAUT_Extrusion; }
    virtual TSharedPtr<SWidget> MakeWidget() override;
    void LoadListData();

private:
    // 点击替换造型线响应
    void Callback_ReplacingExtrusionType();
    // 选中了某个欲替换的造型线
    void Callback_ReplacingExtrusionItem(int32 InItemCode);
    // 获取可替换造型线列表
    void LoadExtrusionData();
    // 填充可替换造型线列表
    void FillExtrusionList(int32 InStart, int32 InCount);

    void SetDownloadExtrusionResState(bool bSucceeded, class SContentItem *UIItem, struct FTableInfo *DataItem);

private:
    int32 ExtrusionType;
};