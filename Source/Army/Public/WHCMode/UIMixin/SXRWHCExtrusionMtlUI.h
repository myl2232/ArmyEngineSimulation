#pragma once
#include "../XRWHCAttrPanelMixin.h"

class FArmyWHCExtrusionMtlUI final : public TXRWHCAttrUIComponent<FArmyWHCExtrusionMtlUI>
{
public:
    FArmyWHCExtrusionMtlUI(class SArmyWHCModeAttrPanelContainer *InContainer, int32 ExtrusionType_)
    : TXRWHCAttrUIComponent<FArmyWHCExtrusionMtlUI>(InContainer)
    , ExtrusionType(ExtrusionType_)
    { Type = EWHCAttrUIType::EWAUT_ExtrusionMtl; }
    virtual TSharedPtr<SWidget> MakeWidget() override;
    void LoadListData();

private:
    // 点击替换踢脚板材质响应
    void Callback_ReplacingExtrusionMtl();
    // 选中了某个欲替换的踢脚板材质
    void Callback_ReplacingExtrusionMtlItem(int32 InItemCode);
    // 获取可替换踢脚板材质列表
    void LoadExtrusionMtlData();
    // 填充可替换踢脚板材质列表
    void FillExtrusionMtlList(int32 InStart, int32 InCount);

    void SetDownloadExtrusionMtlResState(bool bSucceeded, class SContentItem *UIItem, struct FMtlInfo *DataItem);

private:
    int32 ExtrusionType;
};