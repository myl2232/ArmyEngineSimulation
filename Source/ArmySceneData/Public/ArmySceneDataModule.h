#pragma once

#include "Modules/ModuleInterface.h"

class FArmySceneDataModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    ARMYSCENEDATA_API static FArmySceneDataModule& Get();

    /** ����ǽ����ɫ */
    ARMYSCENEDATA_API void SetWallColor(const FLinearColor& NewColor);

    /** ��ȡǽ����ɫ */
    ARMYSCENEDATA_API const FLinearColor& GetWallColor() const { return WallColor; }

private:
    static FArmySceneDataModule* Singleton;

    /** ǽ����ɫ */
    FLinearColor WallColor;
};