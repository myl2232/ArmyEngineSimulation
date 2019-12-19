#pragma once
#include "CoreMinimal.h"
#include "../XRWHCAttrPanelMixin.h"

class FArmyWHCSlidingwayUI final : public TXRWHCAttrUIComponent<FArmyWHCSlidingwayUI>
{
public:
    FArmyWHCSlidingwayUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : TXRWHCAttrUIComponent<FArmyWHCSlidingwayUI>(InContainer)
    { Type = EWHCAttrUIType::EWAUT_Slidingway; }
    virtual TSharedPtr<SWidget> MakeWidget() override;
    void LoadListData();

private:
    // 点击替换柜子门板响应
    void Callback_ReplacingSlidingway();
    // 选中了某个欲替换的门板
    void Callback_ReplacingSlidingwayItem(int32 InItemCode);
    // 获取可替换柜子门板列表
    void LoadSlidingwayData();
    // 填充可替换柜子门板列表
    void FillSlidingwayList(int32 InStart, int32 InCount);
};