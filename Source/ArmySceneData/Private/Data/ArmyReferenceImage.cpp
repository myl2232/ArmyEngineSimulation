#include "ArmyReferenceImage.h"
#include "ArmyRect.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyViewportClient.h"
#include "ArmyEngineModule.h"
#include "ArmyUser.h"
#include "ArmyCommonTools.h"

FArmyReferenceImage::FArmyReferenceImage()
{
	ObjectType = OT_ReferenceImage;

	ImagePanel = MakeShareable(new FArmyRect());
    ImagePanel->DrawBoundray = false;

	ReferenceImageMt = FArmyEngineModule::Get().GetEngineResource()->GetBaseMapMaterial();
	ReferenceImageMt->AddToRoot();
	ReferenceImageDynamicMt = UMaterialInstanceDynamic::Create(ReferenceImageMt,GVC->GetWorld()->GetCurrentLevel());
	ReferenceImageDynamicMt->AddToRoot();
	ImagePanel->MaterialRenderProxy = ReferenceImageDynamicMt->GetRenderProxy(false);
}

void FArmyReferenceImage::SetTexture(UTexture2D* InTexture)
{
	ImagePanel->Width = InTexture->GetSizeX();
	ImagePanel->Height = InTexture->GetSizeY();
	ImagePanel->Pos = FVector(-ImagePanel->Width / 2, -ImagePanel->Height / 2,0);
	ReferenceImageDynamicMt->SetTextureParameterValue(FName("BaseMap"), InTexture);
	ImagePanel->bIsFilled = true;
}

void FArmyReferenceImage::SetTransparency(const int32 InTransparency)
{
    Transparency = InTransparency;
    ReferenceImageDynamicMt->SetScalarParameterValue(FName("Transparency"), InTransparency / 100.f);
}

void FArmyReferenceImage::SetScale(float InScale)
{
    Scale = InScale;
	ImagePanel->Width *= InScale;
	ImagePanel->Height *= InScale;
	ImagePanel->Pos = FVector(-ImagePanel->Width / 2, -ImagePanel->Height / 2, 0);
}

FString FArmyReferenceImage::GetFilename()
{
    return FString::Printf(TEXT("Facsimile_%d.%s"), FArmyUser::Get().GetCurPlanID(), *Suffix);
}

void FArmyReferenceImage::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		ImagePanel->Draw(PDI, View);
	}
}

void FArmyReferenceImage::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    FArmyObject::SerializeToJson(JsonWriter);

    if (FacsimileData.Num() > 0)
    {
        JsonWriter->WriteValue("facsimilePath", FArmyCommonTools::GetFacsimilesDir() + GetFilename());
        JsonWriter->WriteValue("transparency", Transparency);
        JsonWriter->WriteValue("scale", Scale);
    }

    SERIALIZEREGISTERCLASS(JsonWriter, FArmyReferenceImage)
}

void FArmyReferenceImage::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
    FArmyObject::Deserialization(InJsonData);

    FString FacsimilePath;
    InJsonData->TryGetStringField("facsimilePath", FacsimilePath);
    if (!FacsimilePath.IsEmpty())
    {
        double TempScale = 0;
        InJsonData->TryGetNumberField("scale", TempScale);
        Scale = TempScale;
        InJsonData->TryGetNumberField("transparency", Transparency);
        LoadFacsimile(FacsimilePath);
    }
}

bool FArmyReferenceImage::LoadFacsimile(const FString& InFacsimilePath)
{
    UTexture2D* TextureObject = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(InFacsimilePath, false);
    if (TextureObject)
    {
        // @欧石楠 保存临摹图数据，供保存方案使用
        if (FFileHelper::LoadFileToArray(FacsimileData, *InFacsimilePath))
        {
            Suffix = InFacsimilePath.Right(
                InFacsimilePath.Len() - 1 - InFacsimilePath.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromEnd));
        }

        SetTexture(TextureObject);
        SetTransparency(Transparency);
        SetScale(Scale);

        return true;
    }

    return false;
}
