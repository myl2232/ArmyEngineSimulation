#pragma once
#include "SArmyWHCModeAttrPanel.h"

class FArmyWHCModePlatformAttr final : public FArmyWHCModeAttrPanel
{
public:
    FArmyWHCModePlatformAttr(TSharedRef<class SArmyWHCModeAttrPanelContainer> InContainer);
    virtual void TickPanel() override;
    virtual void Initialize() override;
    virtual bool TryDisplayAttrPanel(class AActor *InActor) override;
    virtual void LoadMoreReplacingData() override;

private:
	

    // 点击替换列表中的某项
    void Callback_ItemClicked(int32 InItemCode);

    // 点击替换台面材质响应
    void Callback_ReplacingPlatformMtl();
    // 获取可替换台面材质列表
    void LoadPlatformData();
    // 填充可替换台面材质列表
    void FillPlatformMtlList(int32 InStart, int32 InCount);
    // 选中了某个欲替换的台面材质
    void ReplacingPlatformMtlItemClicked(int32 InItemCode);

    // 点击替换台面前沿响应
    void Callback_ReplacingPlatformFront();
    // 获取可替换台面前沿列表
    void LoadPlatformTrimData();
    // 填充可替换台面前沿列表
    void FillPlatformFrontList();
    // 选中了某个欲替换的台面前沿
    void ReplacingPlatformFrontItemClicked(int32 InItemCode);

    void Callback_PlatformBackMenuItemSelectionChanged(const FString &InMenuItem);

    void SetDownloadPlatformMtlResState(bool bSucceeded, class SContentItem *UIItem, struct FMtlInfo *DataItem);
    void SetDownloadPlatformFrontTrimResState(bool bSucceeded, class SContentItem *UIItem, struct FTableInfo *DataItem);

private:
    TSharedPtr<class FArmyDetailBuilder> PlatformPanel;
    TSharedPtr<SArmyWHCModeReplacingWidget> PlatformMtlReplacing;
    TSharedPtr<SArmyWHCModeReplacingWidget> PlatformFrontReplacing;
    TSharedPtr<SArmyWHCModeReplacingWidgetWithOverlayMenu> PlatformBackReplacing;

    int32 CurrentPlatformId = 0;
};