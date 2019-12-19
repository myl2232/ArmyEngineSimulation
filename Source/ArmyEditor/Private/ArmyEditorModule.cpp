#include "ArmyEditorModule.h"
#include "SlateOptMacros.h"
#include "ArmyThumbnailRenderer.h"
#include "SlateTextures.h"

DEFINE_LOG_CATEGORY(ArmyEditorLog);

FArmyEditorModule* FArmyEditorModule::Singleton = NULL;

void FArmyEditorModule::StartupModule()
{
	Singleton = this;

	ThumbnailRenderer_MaterialInstance = NewObject<UArmyMaterialInstanceThumbnailRenderer>(GEngine);
	ThumbnailRenderer_Texture = NewObject<UArmyTextureThumbnailRenderer>(GEngine);
	ThumbnailRenderer_MaterialInstance->AddToRoot();
	ThumbnailRenderer_Texture->AddToRoot();
	SharedThumbnailRT = new FSlateTextureRenderTarget2DResource(
		FLinearColor::Black, 256, 256,
		PF_B8G8R8A8, SF_Point, TA_Wrap, TA_Wrap, 0.0f
	);
	BeginInitResource(SharedThumbnailRT);
}

void FArmyEditorModule::ShutdownModule()
{
}

ARMYEDITOR_API FArmyEditorModule& FArmyEditorModule::Get()
{
	if (Singleton == NULL)
	{
		check(IsInGameThread());
		FModuleManager::LoadModuleChecked<FArmyEditorModule>("ArmyEditor");
	}
	check(Singleton != NULL);
	return *Singleton;
}

IMPLEMENT_MODULE(FArmyEditorModule, ArmyEditor);
