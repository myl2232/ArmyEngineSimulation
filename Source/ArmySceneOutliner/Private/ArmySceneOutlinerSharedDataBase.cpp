#include "Modules/ModuleManager.h"
#include "ArmySceneOutlinerPublicTypes.h"
#include "ArmySceneOutlinerModule.h"

void FArmySharedDataBase::UseDefaultColumns()
{
    FArmySceneOutlinerModule& SceneOutlinerModule = FModuleManager::LoadModuleChecked<FArmySceneOutlinerModule>("ArmySceneOutliner");

    for (auto& DefaultColumn : SceneOutlinerModule.DefaultColumnMap)
    {
        if (!DefaultColumn.Value.ValidMode.IsSet() || Mode == DefaultColumn.Value.ValidMode.GetValue())
        {
            ColumnMap.Add(DefaultColumn.Key, DefaultColumn.Value.ColumnInfo);
        }
    }
}