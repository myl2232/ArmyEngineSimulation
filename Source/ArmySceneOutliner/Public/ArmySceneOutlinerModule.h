#pragma once

#include "Modules/ModuleInterface.h"
#include "SXRSceneOutliner.h"
#include "ArmySceneOutlinerPublicTypes.h"
#include "ArmySceneOutlinerFwd.h"

class FArmySceneOutlinerModule : public IModuleInterface
{
public:
    //~ Begin IModuleInterface Interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    //~ End IModuleInterface Interface

    XRSCENEOUTLINER_API static FArmySceneOutlinerModule& Get();

    /** 创建场景物体列表 */
    XRSCENEOUTLINER_API TSharedRef<IXRSceneOutliner> CreateSceneOutliner(FArmyInitializationOptions& InitOptions) const;

public:
    /** Register a new type of default column available to all scene outliners */
    template<typename T>
    void RegisterDefaultColumnType(FArmyDefaultColumnInfo InDefaultColumnInfo)
    {
        auto ID = T::GetID();
        if (!ColumnMap.Contains(ID))
        {
            auto CreateColumn = [](IXRSceneOutliner& Outliner) {
                return TSharedRef<IXRSceneOutlinerColumn>(MakeShareable(new T(Outliner)));
            };

            ColumnMap.Add(ID, FArmyCreateSceneOutlinerColumn::CreateStatic(CreateColumn));
            DefaultColumnMap.Add(ID, InDefaultColumnInfo);
        }
    }

    /** Unregister a previously registered column type */
    template<typename T>
    void UnRegisterColumnType()
    {
        ColumnMap.Remove(T::GetID());
        DefaultColumnMap.Remove(T::GetID());
    }

    /** Factory a new column from the specified name. Returns null if no type has been registered under that name. */
    TSharedPtr<IXRSceneOutlinerColumn> FactoryColumn(FName ID, IXRSceneOutliner& Outliner) const
    {
        if (auto* Factory = ColumnMap.Find(ID))
        {
            return Factory->Execute(Outliner);
        }

        return nullptr;
    }

    /** Map of column type name -> default column info */
    TMap<FName, FArmyDefaultColumnInfo> DefaultColumnMap;

private:
    /** Map of column type name -> factory delegate */
    TMap< FName, FArmyCreateSceneOutlinerColumn > ColumnMap;

private:
    static FArmySceneOutlinerModule* Singleton;
};
