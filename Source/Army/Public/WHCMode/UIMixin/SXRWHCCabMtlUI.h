#pragma once
#include "CoreMinimal.h"
#include "../XRWHCAttrPanelMixin.h"

class FArmyWHCCabMtlUI final : public TXRWHCAttrUIComponent<FArmyWHCCabMtlUI>
{
public:
    FArmyWHCCabMtlUI(class SArmyWHCModeAttrPanelContainer *InContainer, bool bNeedRangeReplacing_)
    : TXRWHCAttrUIComponent<FArmyWHCCabMtlUI>(InContainer)\
    , bNeedRangeReplacing(bNeedRangeReplacing_)
    { Type = EWHCAttrUIType::EWAUT_CabMtl; }
    virtual TSharedPtr<SWidget> MakeWidget() override;
    void LoadListData();

private:
    // 点击替换柜子材质
    void Callback_ReplacingCabinetMtl();
    // 选中了某个欲替换的柜子材质
    void Callback_ReplacingCabinetMtlItem(int32 InItemCode);
    // 柜子材质分类改变
    void Callback_CabinetMtlCategoryChanged(const FString &InNewCategory);
    // 获取可替换柜子材质列表
    void LoadCabinetMtlData();
    // 填充柜子材质列表
    void FillCabinetMtlList(int32 InStart, int32 InCount);
    // 获取柜子材质分类列表
    void LoadCabinetMtlCategoryData(int32 InMtlType);
    // 填充柜体材质分类下拉框
    void FillCabinetMtlCategoryComboBox(const TArray<TPair<int32, FString>> &InCabMtlCategory);

    void ReplaceCabMtlInternal(struct FShapeInRoom *InShapeInRoom, struct FMtlInfo *InMtlInfo);
    void UpdateCabMtlUI(struct FShapeInRoom *InShapeInRoom, int32 &OutId, FString &OutThumbnailUrl, FString &OutName);
    class FArmyWHCCabinetMtlChange* GetOrCreateCabinetMtlChange(struct FShapeInRoom *InShapeInRoom) const;
    void SetDownloadCabMtlResState(bool bSucceeded, class SContentItem *UIItem, struct FMtlInfo *DataItem);
    
private:
    TSharedPtr<class SArmyWHCModeNormalComboBox> CabMtlCategoryComboBox;

    TMap<int32, TArray<TPair<int32, FString>>> CabMtlCategoryMap;
    int32 CurrentMtlType;
    int32 CurrentMtlCategory;

    TSharedPtr<class FArmyWHCModificationRangeUI> MRangeUI;
    bool bNeedRangeReplacing = false;
};