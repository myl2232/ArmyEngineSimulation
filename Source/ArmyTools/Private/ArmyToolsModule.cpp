#include "ArmyToolsModule.h"
#include "Modules/ModuleManager.h"
#include "ArmyRectSelect.h"

//#include "ArmyDxfModel.h"
#include "dxfFile.h"

FArmyToolsModule* FArmyToolsModule::Singleton = NULL;

void FArmyToolsModule::StartupModule()
{
    Singleton = this;
	MouseCaptureTool = MakeShareable(new FArmyMouseCapture);
	RectSelectTool = MakeShareable(new FArmyRectSelect);
}

void FArmyToolsModule::ShutdownModule()
{
}

FArmyToolsModule& FArmyToolsModule::Get()
{
    if (Singleton == NULL)
    {
        check(IsInGameThread());
        FModuleManager::LoadModuleChecked<FArmyToolsModule>("ArmyTools");
    }
    check(Singleton != NULL);

    return *Singleton;
}
TSharedPtr<FArmyMouseCapture> FArmyToolsModule::GetMouseCaptureTool()
{
	return MouseCaptureTool;
}
TSharedPtr<FArmyRectSelect> FArmyToolsModule::GetRectSelectTool()
{
	return RectSelectTool;
}
TArray<TSharedPtr<"FArmyFurniture>> FArmyToolsModule::ParseDXF(const FString& InFilePath)
{
	std::string FilePath(TCHAR_TO_UTF8(*InFilePath));
	dxfFile DxfFile(FilePath);
	if (DxfFile.parseFile())
	{
		TSharedPtr<"FArmyDxfModel> DxfModel = MakeShareable(DxfFile.DxfToHomeEditorData());
		return DxfModel->GetAllLayers();
	}
	return TArray<TSharedPtr<"FArmyFurniture>>(NULL,0);
}

// 通过DXF获得图例
TSharedPtr<"FArmyFurniture> FArmyToolsModule::GetFurnitureByDXF(const FString& InFilePath)
{
	TArray<TSharedPtr<"FArmyFurniture>> FurnitureList = ParseDXF(InFilePath);
	if (FurnitureList.Num() > 0)
	{
		return FurnitureList[0];
	}
	else
	{
		return NULL;
	}
}
IMPLEMENT_MODULE(FArmyToolsModule, ArmyTools);