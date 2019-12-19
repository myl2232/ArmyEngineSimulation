#pragma once
#include "CoreMinimal.h"
#include "../XRWHCAttrPanelMixin.h"

class FArmyWHCCabDoorMtlUIBase : public TXRWHCAttrUIComponent<FArmyWHCCabDoorMtlUIBase>
{
public:
    FArmyWHCCabDoorMtlUIBase(class SArmyWHCModeAttrPanelContainer *InContainer)
    : TXRWHCAttrUIComponent<FArmyWHCCabDoorMtlUIBase>(InContainer)
    { Type = EWHCAttrUIType::EWAUT_DoorMtl; }
    virtual TSharedPtr<SWidget> MakeWidget() override;
    void LoadListData();

protected:
    virtual bool IsValidSelection() const = 0;
    virtual class AXRShapeFrame* GetShapeFrame() const = 0;
    virtual void ReplacingDoorMtlItemInternal(int32 InItemCode, struct FShapeInRoom *InShapeInRoom, struct FMtlInfo *InMtlInfo) = 0;
    virtual TSharedRef<SWidget> MakeReplacingListPanelContent() = 0;

private:
    // 点击替换材质响应
    void Callback_ReplacingDoorMtl();
    // 选中了某个欲替换的门板材质
    void Callback_ReplacingDoorMtlItem(int32 InItemCode);
    // 门板材质分类改变
    void Callback_DoorMtlCategoryChanged(const FString &InNewCategory);
    // 获取可替换门板材质列表
    void LoadDoorMtlData();
    // 填充可替换门板材质列表
    void FillDoorMtlList(int32 InStart, int32 InCount);
    // 获取门板材质分类列表
    void LoadDoorMtlCategoryData();
    // 填充门板材质分类的下拉框
    void FillDoorMtlCategoryComboBox(const TArray<TPair<int32, FString>> &InDoorMtlCategory);
    
protected:
    TArray<class FDoorGroup*> GetDoorGroups(struct FShapeInRoom *InShapeInRoom) const;
    class FDoorSheetShapeBase* GetFirstDoorSheet(struct FShapeInRoom *InShapeInRoom) const;
    class FArmyWHCSingleDoorGroupMtlChange* GetOrCreateDoorMtlChange(struct FShapeInRoom *InShapeInRoom, const FGuid &InGuid) const;
    class FArmyWHCSingleDrawerGroupMtlChange* GetOrCreateDrawerMtlChange(struct FShapeInRoom *InShapeInRoom, const FGuid &InGuid) const;
    void SetDownloadDoorMtlResState(bool bSucceeded, class SContentItem *UIItem, struct FMtlInfo *DataItem);

protected:
    TSharedPtr<class SArmyWHCModeNormalComboBox> DoorMtlCategoryComboBox;

private:
    TMap<int32, TArray<TPair<int32, FString>>> DoorMtlCategoryMap;
    int32 CurrentDoorMtlCategory;
};

class FArmyWHCCabDoorMtlUI final : public FArmyWHCCabDoorMtlUIBase
{
public:
    FArmyWHCCabDoorMtlUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : FArmyWHCCabDoorMtlUIBase(InContainer) {}

protected:
    virtual bool IsValidSelection() const override;
    virtual class AXRShapeFrame* GetShapeFrame() const override;  
    virtual void ReplacingDoorMtlItemInternal(int32 InItemCode, struct FShapeInRoom *InShapeInRoom, struct FMtlInfo *InMtlInfo) override;
    virtual TSharedRef<SWidget> MakeReplacingListPanelContent() override;
};

class FArmyWHCDoorMtlUI final : public FArmyWHCCabDoorMtlUIBase
{
public:
    FArmyWHCDoorMtlUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : FArmyWHCCabDoorMtlUIBase(InContainer) {}
    virtual TSharedPtr<SWidget> MakeWidget() override;  

protected:
    virtual bool IsValidSelection() const override;
    virtual class AXRShapeFrame* GetShapeFrame() const override;  
    virtual void ReplacingDoorMtlItemInternal(int32 InItemCode, struct FShapeInRoom *InShapeInRoom, struct FMtlInfo *InMtlInfo) override;
    virtual TSharedRef<SWidget> MakeReplacingListPanelContent() override;

private:
    void ReplacingDoorMtl(struct FShapeInRoom *InShapeInRoom, class FSCTShape *InDoorGroupToReplace, struct FMtlInfo *InMtlInfo);
    void ReplacingSingleDoorOrDrawerMtl(struct FShapeInRoom *InShapeInRoom, struct FMtlInfo *InMtlInfo);
    void ReplacingSingleCabDoorOrDrawerMtl(struct FShapeInRoom *InShapeInRoom, struct FMtlInfo *InMtlInfo);
    void ReplacingSameTypeCabDoorOrDrawerMtl(struct FShapeInRoom *InShapeInRoom, struct FMtlInfo *InMtlInfo);
    void ReplacingAllCabDoorOrDrawerMtl(struct FShapeInRoom *InShapeInRoom, struct FMtlInfo *InMtlInfo);
    void ReplacingInsertionBoardMtl(struct FShapeInRoom *InShapeInRoom, struct FMtlInfo *InMtlInfo);
    class FArmyWHCCabinetInsertionBoardMtlChange* GetOrCreateInsertionBoardMtlChange(struct FShapeInRoom *InShapeInRoom);
    class FSCTShape* GetCurrentDoorGroup();
    class FSCTShape* GetDoorSheet(class FDoorGroup *InDoorGroup);

private:
    TSharedPtr<class FArmyWHCModificationRangeUI> MRangeUI;
};