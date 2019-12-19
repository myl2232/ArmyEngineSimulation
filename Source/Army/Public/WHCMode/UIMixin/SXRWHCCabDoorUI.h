#pragma once
#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "../XRWHCAttrPanelMixin.h"

class FArmyWHCCabDoorUIBase : public TXRWHCAttrUIComponent<FArmyWHCCabDoorUIBase>
{
public:
    FArmyWHCCabDoorUIBase(class SArmyWHCModeAttrPanelContainer *InContainer, int32 InDoorType)
    : TXRWHCAttrUIComponent<FArmyWHCCabDoorUIBase>(InContainer)
    , CurrentDoorType(InDoorType)
    { Type = EWHCAttrUIType::EWAUT_Door; }
    virtual TSharedPtr<SWidget> MakeWidget() override;
    void LoadListData();

protected:
    virtual bool IsValidSelection() const = 0;
    virtual class AXRShapeFrame* GetShapeFrame() const = 0;
    virtual void ReplacingDoorItemInternal(int32 InItemCode, struct FShapeInRoom *InShapeInRoom, struct FCoverDoorTypeInfo *InDoorTypeInfo) = 0;
    virtual TSharedRef<SWidget> MakeReplacingListPanelContent() = 0;

    int32 GetCurrentDoorType() const { return CurrentDoorType; }

private:
    // 点击替换柜子门板响应
    void Callback_ReplacingDoor();
    // 选中了某个欲替换的门板
    void Callback_ReplacingDoorItem(int32 InItemCode);
    // 门板分类改变
    void Callback_DoorCategoryChanged(const FString &InNewCategory);
    // 获取可替换柜子门板列表
    void LoadDoorData();
    // 填充可替换柜子门板列表
    void FillDoorList(int32 InStart, int32 InCount);
    // 欲替换的门板资源数据下载完成
    void ReplacingDoorResourcesDownloadFinished(bool bSucceeded, int32 InShapeId);
    // 获取门板分类列表
    void LoadDoorCategoryData();
    // 填充门板分类下拉框
    void FillDoorCategoryComboBox();

protected:
    TArray<class FDoorGroup*> GetDoorGroups(struct FShapeInRoom *InShapeInRoom) const;
    class FDoorGroup* GetFirstDoorGroup(struct FShapeInRoom *InShapeInRoom) const;
    class FArmyWHCSingleDoorGroupChange* GetOrCreateDoorChange(struct FShapeInRoom *InShapeInRoom, const FGuid &InGuid) const;
    class FArmyWHCSingleDrawerGroupChange* GetOrCreateDrawerChange(struct FShapeInRoom *InShapeInRoom, const FGuid &InGuid) const;
    void SetDownloadDoorResState(bool bSucceeded, class SContentItem *UIItem, struct FCoverDoorTypeInfo *DataItem);

protected:
    TSharedPtr<class SArmyWHCModeNormalComboBox> CabDoorCategoryComboBox;

private:
    TArray<TPair<int32, FString>> DoorTypeCategoryArr;
    int32 CurrentDoorTypeCategory;

    // 1 - 移门
    // 2 - 掩门
    // 3 - 抽面
    int32 CurrentDoorType = -1;
};

class FArmyWHCCabDoorUI final : public FArmyWHCCabDoorUIBase
{
public:
    FArmyWHCCabDoorUI(class SArmyWHCModeAttrPanelContainer *InContainer, int32 InDoorType)
    : FArmyWHCCabDoorUIBase(InContainer, InDoorType) {}

protected:
    virtual bool IsValidSelection() const override; 
    virtual class AXRShapeFrame* GetShapeFrame() const override;
    virtual void ReplacingDoorItemInternal(int32 InItemCode, struct FShapeInRoom *InShapeInRoom, struct FCoverDoorTypeInfo *InDoorTypeInfo) override;
    virtual TSharedRef<SWidget> MakeReplacingListPanelContent() override;
};

class FArmyWHCDoorUI final : public FArmyWHCCabDoorUIBase
{
public:
    FArmyWHCDoorUI(class SArmyWHCModeAttrPanelContainer *InContainer, int32 InDoorType)
    : FArmyWHCCabDoorUIBase(InContainer, InDoorType) {}
    virtual TSharedPtr<SWidget> MakeWidget() override;

protected:
    virtual bool IsValidSelection() const override;
    virtual class AXRShapeFrame* GetShapeFrame() const override;
    virtual void ReplacingDoorItemInternal(int32 InItemCode, struct FShapeInRoom *InShapeInRoom, struct FCoverDoorTypeInfo *InDoorTypeInfo) override;
    virtual TSharedRef<SWidget> MakeReplacingListPanelContent() override;

private:
    class FSCTShape* ReplacingDoorOrDrawer(struct FShapeInRoom *InShapeInRoom, class FSCTShape *InDoorSheetToReplace, struct FCoverDoorTypeInfo *InDoorTypeInfo);
    void ReplacingSingleDoorOrDrawer(struct FShapeInRoom *InShapeInRoom, class FSCTShape *InShapeToProcess, struct FCoverDoorTypeInfo *InDoorTypeInfo);
    void ReplacingSingleCabDoorOrDrawer(struct FShapeInRoom *InShapeInRoom, class FSCTShape *InShapeToProcess, struct FCoverDoorTypeInfo *InDoorTypeInfo, bool bNeedUpdateSelected = true);
    void ReplacingSameTypeCabDoorOrDrawer(struct FShapeInRoom *InShapeInRoom, struct FCoverDoorTypeInfo *InDoorTypeInfo);
    void ReplacingAllCabDoorOrDrawer(struct FShapeInRoom *InShapeInRoom, struct FCoverDoorTypeInfo *InDoorTypeInfo);
    void ReplacingInsertionBoardMtl(struct FShapeInRoom *InShapeInRoom, class FSCTShape *InDoorSheetToPeekMtl);
    class FArmyWHCCabinetInsertionBoardMtlChange* GetOrCreateInsertionBoardMtlChange(struct FShapeInRoom *InShapeInRoom);

private:
    TSharedPtr<class FArmyWHCModificationRangeUI> MRangeUI;
};