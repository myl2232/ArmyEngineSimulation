#pragma once
#include "SArmyWHCModeAttrPanel.h"

class FArmyWHCModeEmbbedElecDevAttr final : public FArmyWHCModeAttrPanel
{
public:
    FArmyWHCModeEmbbedElecDevAttr(TSharedRef<class SArmyWHCModeAttrPanelContainer> InContainer);
    virtual void TickPanel() override;
    virtual void Initialize() override;
    virtual bool TryDisplayAttrPanel(class AActor *InActor) override;
    virtual void LoadMoreReplacingData() override;

private:
    void Callback_ItemClicked(int32 InItemCode);
    // 点击替换嵌入式电器响应
    void Callback_ReplacingEmbbedElecDev();
    // 获取可替换嵌入式电器
    void LoadEmbbedElecDevData();
    // 填充可替换嵌入式电器列表
    void FillEmbbedElecDevList(int32 InStart, int32 InCount);
    // 删除嵌入式电器
    void Callback_DeleteEmbbedElecDev();
    void ReplacingEmbbedElecDevItemClicked(int32 InItemCode);

    class FArmyWHCEmbbedElecDevChange* GetOrCreateEmbbedElecDevChange(struct FShapeInRoom *InShapeInRoom) const;

    void SetDownloadAccResState(bool bSucceeded, class SContentItem *UIItem, struct FStandaloneCabAccInfo *DataItem);

private:
    TSharedPtr<class FArmyDetailBuilder> EmbbedElecDevDetailPanel;
    TSharedPtr<SArmyWHCModeReplacingWidget> EmbbedElecDevReplacingPanel;
    TSharedPtr<STextBlock> EmbbedElecDevWidthText;
    TSharedPtr<STextBlock> EmbbedElecDevDepthText;
    TSharedPtr<STextBlock> EmbbedElecDevHeightText;
    TSharedPtr<STextBlock> InsertionBoardText;
};