#include "ArmySceneOutlinerModule.h"
#include "ArmySceneOutlinerGutter.h"
#include "ArmySceneOutlinerSelectableGutter.h"
#include "ArmySceneOutlinerItemLabelColumn.h"

IMPLEMENT_MODULE(FArmySceneOutlinerModule, XRSceneOutliner);

FArmySceneOutlinerModule* FArmySceneOutlinerModule::Singleton = NULL;

void FArmySceneOutlinerModule::StartupModule()
{
    Singleton = this;

    RegisterDefaultColumnType<FArmySceneOutlinerGutter>(FArmyDefaultColumnInfo(FArmyColumnInfo(EXRColumnVisibility::Visible, 0), EXRSceneOutlinerMode::ActorBrowsing));
	RegisterDefaultColumnType<FArmyItemLabelColumn>(FArmyDefaultColumnInfo(FArmyColumnInfo(EXRColumnVisibility::Visible, 10)));
	RegisterDefaultColumnType<FArmySceneOutlinerSelectableGutter>(FArmyDefaultColumnInfo(FArmyColumnInfo(EXRColumnVisibility::Visible, 11), EXRSceneOutlinerMode::ActorBrowsing));
}

void FArmySceneOutlinerModule::ShutdownModule()
{
	UnRegisterColumnType<FArmySceneOutlinerGutter>();
	UnRegisterColumnType<FArmyItemLabelColumn>();
	UnRegisterColumnType<FArmySceneOutlinerSelectableGutter>();
}

XRSCENEOUTLINER_API TSharedRef<IXRSceneOutliner> FArmySceneOutlinerModule::CreateSceneOutliner(FArmyInitializationOptions& InitOptions) const
{
    return SNew(SXRSceneOutliner, InitOptions);
}

XRSCENEOUTLINER_API FArmySceneOutlinerModule& FArmySceneOutlinerModule::Get()
{
    if (Singleton == NULL)
    {
        check(IsInGameThread());
        FModuleManager::LoadModuleChecked<FArmySceneOutlinerModule>("ArmySceneOutliner");
    }
    check(Singleton != NULL);

    return *Singleton;
}
