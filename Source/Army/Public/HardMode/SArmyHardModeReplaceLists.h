#pragma once

#include "SArmyReplaceList.h"

class SArmyDoorReplaceList : public SArmyReplaceList
{
public:
    void Construct(const FArguments& InArgs);

    /** 设置尺寸筛选项 */
    void SetFilterSize(const float InLength, const float InHeight);
    virtual void RebuildFullUrl() override;

protected:
    virtual TSharedRef<SWidget> CreateFilterItems() override;

protected:
    /** 是否适配尺寸 */
    bool bFitSize = false;

    float Length;

    float Height;
};