#pragma once

#include "ArmyCommonTypes.h"

class IArmyReplace
{
    /** 替换商品 */
    virtual void ReplaceGoods(FContentItemPtr InContentItem, UWorld* InWorld) = 0;
};