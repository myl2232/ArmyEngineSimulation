#pragma once

#include "Modules/ModuleInterface.h"

class FArmySceneDataModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    ARMYSCENEDATA_API static FArmySceneDataModule& Get();

    /** 设置墙体颜色 */
    ARMYSCENEDATA_API void SetWallColor(const FLinearColor& NewColor);

    /** 获取墙体颜色 */
    ARMYSCENEDATA_API const FLinearColor& GetWallColor() const { return WallColor; }

private:
    static FArmySceneDataModule* Singleton;

    /** 墙体颜色 */
    FLinearColor WallColor;
};