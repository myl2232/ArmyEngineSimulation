#pragma once
#include "CoreMinimal.h"
#include "../XRWHCAttrPanelMixin.h"

class FArmyWHCCabUI : public TXRWHCAttrUIComponent<FArmyWHCCabUI>
{
public:
    FArmyWHCCabUI(class SArmyWHCModeAttrPanelContainer *InContainer, int32 InClassify)
    : TXRWHCAttrUIComponent<FArmyWHCCabUI>(InContainer)
    , CurrentClassify(InClassify)
    { Type = EWHCAttrUIType::EWAUT_Cab; }
    virtual TSharedPtr<SWidget> MakeWidget() override;
    void LoadListData();

protected:
    virtual FString MakeQueryUrl(int32 InType, int32 InWidth, int32 InDepth, int32 InHeight) const;
    virtual void ReplacingCabInternal(const TSharedPtr<class FSCTShape> &InTemplateShape, struct FShapeInRoom *InShapeInRoom, struct FCabinetInfo *InCabInfo, struct FCabinetWholeComponent *InCabWholeComponent);
    int32 GetCurrentClassify() const { return CurrentClassify; }

private:
    // 点击替换柜子响应
    void Callback_ReplacingCabinet();
    // 选中了某个欲替换的柜子
    void Callback_ReplacingCabinetItem(int32 InItemCode);
    // 获取可替换柜子列表
    void LoadCabinetData();
    // 填充可替换柜子列表
    void FillCabinetList(int32 InStart, int32 InCount);
    // 欲替换的柜子Json数据下载完成
    void ReplacingCabinetJsonDownloadFinished(bool bSucceeded, FString InJsonFileCachePath, int32 Id);
    // 欲替换的柜子资源数据下载完成
    void ReplacingCabinetResourcesDownloadFinished(bool bSucceeded, FString InJsonFileCachePath, int32 InShapeId);

    void SetDownloadCabResState(bool bSucceeded, class SContentItem *UIItem, struct FCabinetInfo *DataItem);

private:
    int32 CurrentClassify = -1;
};

class FArmyWHCVentilatorCabUI final : public FArmyWHCCabUI
{
public:
    FArmyWHCVentilatorCabUI(class SArmyWHCModeAttrPanelContainer *InContainer, int32 InClassify)
    : FArmyWHCCabUI(InContainer, InClassify) {}

protected:
    virtual FString MakeQueryUrl(int32 InType, int32 InWidth, int32 InDepth, int32 InHeight) const override;
};

class FArmyWHCBathroomCabUI final : public FArmyWHCCabUI
{
public:
    FArmyWHCBathroomCabUI(class SArmyWHCModeAttrPanelContainer *InContainer, int32 InClassify)
    : FArmyWHCCabUI(InContainer, InClassify) {}

protected:
    virtual FString MakeQueryUrl(int32 InType, int32 InWidth, int32 InDepth, int32 InHeight) const override;
    virtual void ReplacingCabInternal(const TSharedPtr<class FSCTShape> &InTemplateShape, struct FShapeInRoom *InShapeInRoom, struct FCabinetInfo *InCabInfo, struct FCabinetWholeComponent *InCabWholeComponent) override;
};