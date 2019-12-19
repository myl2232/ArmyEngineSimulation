#include "ArmySceneDataModule.h"

#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FArmySceneDataModule, XRHomeData);

FArmySceneDataModule* FArmySceneDataModule::Singleton = NULL;

void FArmySceneDataModule::StartupModule()
{
    Singleton = this;
    WallColor = FLinearColor::White;
}

void FArmySceneDataModule::ShutdownModule()
{
}


ARMYSCENEDATA_API FArmySceneDataModule& FArmySceneDataModule::Get()
{
    if (Singleton == NULL)
    {
        check(IsInGameThread());
        FModuleManager::LoadModuleChecked<FArmySceneDataModule>("ArmySceneData");
    }
    check(Singleton != NULL);

    return *Singleton;
}

ARMYSCENEDATA_API void FArmySceneDataModule::SetWallColor(const FLinearColor& NewColor)
{
    WallColor = NewColor;
}