#pragma once
#include "CoreMinimal.h"
#include "../XRWHCAttrPanelMixin.h"

class FArmyWHCCabDoorHandleUIBase : public TXRWHCAttrUIComponent<FArmyWHCCabDoorHandleUIBase>
{
public:
    FArmyWHCCabDoorHandleUIBase(class SArmyWHCModeAttrPanelContainer *InContainer)
    : TXRWHCAttrUIComponent<FArmyWHCCabDoorHandleUIBase>(InContainer)
    { Type = EWHCAttrUIType::EWAUT_DoorHandle; }
    virtual TSharedPtr<SWidget> MakeWidget() override;
    void LoadListData();

protected:
    virtual bool IsValidSelection() const = 0;
    virtual class AXRShapeFrame* GetShapeFrame() const = 0;
    virtual void ReplacingDoorHandleItemInternal(int32 InItemCode, struct FShapeInRoom *InShapeInRoom, int32 Id) = 0;
    virtual TSharedRef<SWidget> MakeReplacingListPanelContent() = 0;

private:
    // 点击替换拉手响应
    void Callback_ReplacingHandle();
    // 选中了某个欲替换的门板拉手
    void Callback_ReplacingDoorHandleItem(int32 InItemCode);
    // 可替换拉手分类改变
    void Callback_DoorHandleCategoryChanged(const FString &InNewCategory);
    // 获取可替换的拉手
    void LoadDoorHandleData();
    // 填充可替换门板拉手列表
    void FillDoorHandleList(int32 InStart, int32 InCount, int64 InShapeId);
    // 获取可替换拉手分类列表
    void LoadDoorHandleCategoryData();
    // 填充可替换拉手分类的下拉框
    void FillDoorHandleCategoryComboBox(const TArray<TPair<int32, FString>> &InDoorHandleCategory);
	// 可替换拉手下载完成
	void ReplacingDoorHandleResourcesDownloadFinished(bool bSucceeded, int32 InShapeId);

protected:
    TArray<class FDoorGroup*> GetDoorGroups(struct FShapeInRoom *InShapeInRoom) const;
    class FDoorGroup* GetFirstDoorGroup(struct FShapeInRoom *InShapeInRoom) const;
    int32 GetDoorIdForHandle(class FDoorGroup *InDoorGroup) const;
    int32 GetCurrentHandleId(class FDoorGroup *InDoorGroup) const;
    const TMap<int32, TSharedPtr<class FSCTShape>>* GetHandleOptionMap(class FDoorGroup *InDoorGroup) const;
    class FArmyWHCSingleDoorGroupHandleChange* GetOrCreateDoorHandleChange(struct FShapeInRoom *InShapeInRoom, const FGuid &InGuid) const;
    class FArmyWHCSingleDrawerGroupHandleChange* GetOrCreateDrawerHandleChange(struct FShapeInRoom *InShapeInRoom, const FGuid &InGuid) const;
    void SetDownloadDoorHandleResState(bool bSucceeded, class SContentItem *UIItem);

protected:
    TSharedPtr<class SArmyWHCModeNormalComboBox> DoorHandleCategoryComboBox;

private:
    TMap<int32, TArray<TPair<int32, FString>>> DoorHandleCategoryMap;
    int32 CurrentDoorHandleCategory;
};

class FArmyWHCCabDoorHandleUI final : public FArmyWHCCabDoorHandleUIBase
{
public:
    FArmyWHCCabDoorHandleUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : FArmyWHCCabDoorHandleUIBase(InContainer) {}

protected:
    virtual bool IsValidSelection() const override;
    virtual class AXRShapeFrame* GetShapeFrame() const override;
    virtual void ReplacingDoorHandleItemInternal(int32 InItemCode, struct FShapeInRoom *InShapeInRoom, int32 Id) override;
    virtual TSharedRef<SWidget> MakeReplacingListPanelContent() override;    
};

class FArmyWHCDoorHandleUI final : public FArmyWHCCabDoorHandleUIBase
{
public:
    FArmyWHCDoorHandleUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : FArmyWHCCabDoorHandleUIBase(InContainer) {}
    virtual TSharedPtr<SWidget> MakeWidget() override;

protected:
    virtual bool IsValidSelection() const override;
    virtual class AXRShapeFrame* GetShapeFrame() const override;
    virtual void ReplacingDoorHandleItemInternal(int32 InItemCode, struct FShapeInRoom *InShapeInRoom, int32 Id) override;
    virtual TSharedRef<SWidget> MakeReplacingListPanelContent() override;    

private:
    void ReplacingDoorHandle(struct FShapeInRoom *InShapeInRoom, class FSCTShape *InDoorGroupToReplace, int32 Id);
    void ReplacingSingleDoorOrDrawerHandle(struct FShapeInRoom *InShapeInRoom, int32 Id);
    void ReplacingSingleCabDoorOrDrawerHandle(struct FShapeInRoom *InShapeInRoom, int32 Id);
    void ReplacingSameTypeCabDoorOrDrawerHandle(struct FShapeInRoom *InShapeInRoom, int32 Id);
    void ReplacingAllCabDoorOrDrawerHandle(struct FShapeInRoom *InShapeInRoom, int32 Id);
    class FSCTShape* GetCurrentDoorGroup();

private:
    TSharedPtr<class FArmyWHCModificationRangeUI> MRangeUI;
};