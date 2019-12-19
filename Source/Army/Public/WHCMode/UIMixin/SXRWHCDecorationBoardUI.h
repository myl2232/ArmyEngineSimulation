#pragma once
#include "../XRWHCAttrPanelMixin.h"

class FArmyWHCDecorationBoardUI final : public TXRWHCAttrUIComponent<FArmyWHCDecorationBoardUI>
{
public:
    FArmyWHCDecorationBoardUI(class SArmyWHCModeAttrPanelContainer *InContainer, int32 Direction_)
    : TXRWHCAttrUIComponent<FArmyWHCDecorationBoardUI>(InContainer)
    , Direction(Direction_)
    { Type = EWHCAttrUIType::EWAUT_DecorationBoard; }
    virtual TSharedPtr<SWidget> MakeWidget() override;
    void LoadListData();
    void SetAddOrReplace(bool bAddOrReplace_) { bAddOrReplace = bAddOrReplace_; }
    bool IsAdd() const { return bAddOrReplace; }
    int32 GetDirection() const { return Direction; }

private:
    // 点击替换柜子见光板材质响应
    void Callback_ReplacingDecorationBoardMtl();
    // 选中了某个欲替换的材质
    void Callback_ReplacingDecorationBoardMtlItem(int32 InItemCode);
    // 点击添加见光板
    FReply Callback_AddDecorationBoard();
    // 点击删除见光板
    void Callback_DeleteDecorationBoard();
    // 获取可替换材质列表
    void LoadMtlData();
    // 填充材质列表
    void FillMtlList(int32 InStart, int32 InCount);

    void SetDownloadMtlResState(bool bSucceeded, class SContentItem *UIItem, struct FMtlInfo *DataItem);

private:
    /**
     * 0 - Left
     * 1 - Right
     */
    int32 Direction;
    bool bAddOrReplace = false;
};