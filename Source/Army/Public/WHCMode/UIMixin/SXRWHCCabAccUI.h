#pragma once
#include "CoreMinimal.h"
#include "../XRWHCAttrPanelMixin.h"

class FArmyWHCAccUIBase : public TXRWHCAttrUIComponent<FArmyWHCAccUIBase>
{
public:
    FArmyWHCAccUIBase(class SArmyWHCModeAttrPanelContainer *InContainer, bool bTopCategory_, bool bCanDelete_)
    : TXRWHCAttrUIComponent<FArmyWHCAccUIBase>(InContainer)
    , bTopCategory(bTopCategory_)
    , bCanDelete(bCanDelete_)
    { Type = EWHCAttrUIType::EWAUT_CabAcc; }
    virtual TSharedPtr<SWidget> MakeWidget() override;
    void LoadListData();
    void SetAddOrReplace(bool bAddOrReplace_) { bAddOrReplace = bAddOrReplace_; }
    bool IsTopCategory() const { return bTopCategory; }
    bool IsAdd() const { return bAddOrReplace; }
    bool IsCanDelete() const { return bCanDelete; }

protected:
    virtual bool IsValidSelection() const = 0;
    virtual class AXRShapeFrame* GetShapeFrame() const = 0;
    virtual void AfterAccReplaced(struct FShapeInRoom *InShapeInRoom, struct FCabinetWholeAccInfo *InAccInfo) = 0;
    virtual void ReplacingAccItemInternal(int32 InItemCode, struct FShapeInRoom *InShapeInRoom, struct FCabinetWholeAccInfo *InAccInfo);

private:
    // 点击替换柜子附属件响应
    void Callback_ReplacingAcc();
    // 选中了某个欲替换的柜子附属件
    void Callback_ReplacingAccItem(int32 InItemCode);
    // 点击添加子部件响应
    FReply Callback_AddAcc();
    // 点击删除柜子附属件响应
    void Callback_DeleteAcc();
    // 获取可替换柜子附属件列表
    void LoadAccData();
    // 填充可替换柜子附属件列表
    void FillAccList(int32 InStart, int32 InCount);

    FString MakeQueryUrl(struct FShapeInRoom *InShapeInRoom) const;
    void SetDownloadAccResState(bool bSucceeded, class SContentItem *UIItem, struct FCabinetWholeAccInfo *DataItem);

private:
    bool bTopCategory;
    bool bAddOrReplace = false;
    bool bCanDelete;
};

class FArmyWHCCabAccUI final : public FArmyWHCAccUIBase
{
public:
    FArmyWHCCabAccUI(class SArmyWHCModeAttrPanelContainer *InContainer, bool bTopCategory_, bool bCanDelete_)
    : FArmyWHCAccUIBase(InContainer, bTopCategory_, bCanDelete_)
    {}

protected:
    virtual bool IsValidSelection() const override;
    virtual class AXRShapeFrame* GetShapeFrame() const override;
    virtual void AfterAccReplaced(struct FShapeInRoom *InShapeInRoom, struct FCabinetWholeAccInfo *InAccInfo) override {}
};

class FArmyWHCAccUI : public FArmyWHCAccUIBase
{
public:
    FArmyWHCAccUI(class SArmyWHCModeAttrPanelContainer *InContainer, bool bTopCategory_, bool bCanDelete_)
    : FArmyWHCAccUIBase(InContainer, bTopCategory_, bCanDelete_)
    {}

protected:
    virtual bool IsValidSelection() const override;
    virtual class AXRShapeFrame* GetShapeFrame() const override;
    virtual void AfterAccReplaced(struct FShapeInRoom *InShapeInRoom, struct FCabinetWholeAccInfo *InAccInfo) override;
};

class FArmyWHCVentilatorUI final : public FArmyWHCAccUI
{
public:
    FArmyWHCVentilatorUI(class SArmyWHCModeAttrPanelContainer *InContainer, bool bTopCategory_, bool bCanDelete_)
    : FArmyWHCAccUI(InContainer, bTopCategory_, bCanDelete_)
    {}

protected:
    virtual void ReplacingAccItemInternal(int32 InItemCode, struct FShapeInRoom *InShapeInRoom, struct FCabinetWholeAccInfo *InAccInfo) override;
};