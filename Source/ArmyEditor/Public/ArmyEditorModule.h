#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(ArmyEditorLog, Log, All);

class FArmyEditorModule : public IModuleInterface
{

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	ARMYEDITOR_API static FArmyEditorModule& Get();

	//用来渲染缩略图的Renderer以及RenderTarget
	UPROPERTY()
	class UArmyMaterialInstanceThumbnailRenderer* ThumbnailRenderer_MaterialInstance;

	UPROPERTY()
	class UArmyTextureThumbnailRenderer* ThumbnailRenderer_Texture;

	class FSlateTextureRenderTarget2DResource* SharedThumbnailRT;

private:
	static FArmyEditorModule* Singleton;
};
