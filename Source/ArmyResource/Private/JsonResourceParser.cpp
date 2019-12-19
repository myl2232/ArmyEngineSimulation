#include "JsonResourceParser.h"
#include "ArmyPointLightActor.h"
#include "ArmySpotLightActor.h"
#include "ArmyReflectionCaptureActor.h"
#include "ArmyLevelAssetBoardActor.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SphereReflectionCaptureComponent.h"
#include "ArmyResourceModule.h"
#include "ResManager.h"

void FJsonResourceParer::Write_PointLight(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter, AArmyPointLightActor* _TargetActor, AActor* _BaseActor)
{
	JsonWriter->WriteObjectStart();
	JsonWriter->WriteValue(TEXT("location"), _BaseActor ? (_TargetActor->GetActorLocation() - _BaseActor->GetActorLocation()).ToString() : _TargetActor->GetActorLocation().ToString());
	JsonWriter->WriteValue(TEXT("intensity"), _TargetActor->PointLightCOM->Intensity);
	JsonWriter->WriteValue(TEXT("radius"), _TargetActor->PointLightCOM->AttenuationRadius);
	JsonWriter->WriteValue(TEXT("color"), _TargetActor->PointLightCOM->GetLightColor().ToString());
	JsonWriter->WriteValue(TEXT("sourceRadius"), _TargetActor->PointLightCOM->SourceRadius);
	JsonWriter->WriteValue(TEXT("sourceLength"), _TargetActor->PointLightCOM->SourceLength);
	JsonWriter->WriteValue(TEXT("castShadow"), _TargetActor->PointLightCOM->CastShadows ? 1 : 0);
	JsonWriter->WriteValue(TEXT("enable"), _TargetActor->PointLightCOM->bVisible ? 1 : 0);
	JsonWriter->WriteValue(TEXT("lightmapid"), _TargetActor->PointLightCOM->LightGuid.ToString());
	JsonWriter->WriteValue(TEXT("mobility"), _TargetActor->GetRootComponent()->Mobility == EComponentMobility::Static ? TEXT("Static") : TEXT("Movable"));
	JsonWriter->WriteValue(TEXT("indirectlightingintensity"), _TargetActor->PointLightCOM->IndirectLightingIntensity);
	JsonWriter->WriteObjectEnd();
}

void FJsonResourceParer::Write_SpotLight(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter, AArmySpotLightActor* _TargetActor, AActor* _BaseActor)
{
	JsonWriter->WriteObjectStart();
	JsonWriter->WriteValue(TEXT("location"), _BaseActor ? (_TargetActor->GetActorLocation() - _BaseActor->GetActorLocation()).ToString() : _TargetActor->GetActorLocation().ToString());
	JsonWriter->WriteValue(TEXT("rotation"), _BaseActor ? (_TargetActor->GetActorRotation() - _BaseActor->GetActorRotation()).ToString() : _TargetActor->GetActorRotation().ToString());
	JsonWriter->WriteValue(TEXT("intensity"), _TargetActor->SpotLightCOM->Intensity);
	JsonWriter->WriteValue(TEXT("radius"), _TargetActor->SpotLightCOM->AttenuationRadius);
	JsonWriter->WriteValue(TEXT("color"), _TargetActor->SpotLightCOM->GetLightColor().ToString());
	JsonWriter->WriteValue(TEXT("sourceRadius"), _TargetActor->SpotLightCOM->SourceRadius);
	JsonWriter->WriteValue(TEXT("sourceLength"), _TargetActor->SpotLightCOM->SourceLength);
	JsonWriter->WriteValue(TEXT("innerAngle"), _TargetActor->SpotLightCOM->InnerConeAngle);
	JsonWriter->WriteValue(TEXT("outerAngle"), _TargetActor->SpotLightCOM->OuterConeAngle);
	JsonWriter->WriteValue(TEXT("castShadow"), _TargetActor->SpotLightCOM->CastShadows ? 1 : 0);
	JsonWriter->WriteValue(TEXT("enable"), _TargetActor->SpotLightCOM->bVisible ? 1 : 0);
	JsonWriter->WriteValue(TEXT("lightmapid"), _TargetActor->SpotLightCOM->LightGuid.ToString());
	JsonWriter->WriteValue(TEXT("mobility"), _TargetActor->GetRootComponent()->Mobility == EComponentMobility::Static ? TEXT("Static") : TEXT("Movable"));
	JsonWriter->WriteValue(TEXT("indirectlightingintensity"), _TargetActor->SpotLightCOM->IndirectLightingIntensity);
	JsonWriter->WriteObjectEnd();
}

void FJsonResourceParer::Write_ReflectionCapture(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter, AArmyReflectionCaptureActor* _TargetActor, AActor* _BaseActor)
{
	JsonWriter->WriteObjectStart();
	JsonWriter->WriteValue(TEXT("location"), _BaseActor ? (_TargetActor->GetActorLocation() - _BaseActor->GetActorLocation()).ToString() : _TargetActor->GetActorLocation().ToString());
	JsonWriter->WriteValue(TEXT("radius"), _TargetActor->ReflectionCaptureCOM->InfluenceRadius);
	JsonWriter->WriteValue(TEXT("brightness"), _TargetActor->ReflectionCaptureCOM->Brightness);
	JsonWriter->WriteObjectEnd();
}

void FJsonResourceParer::Write_LevelAssetBoard(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter, AArmyLevelAssetBoardActor* _TargetActor, AActor* _BaseActor)
{
	JsonWriter->WriteObjectStart();
	JsonWriter->WriteValue(TEXT("location"), _BaseActor ? (_TargetActor->GetActorLocation() - _BaseActor->GetActorLocation()).ToString() : _TargetActor->GetActorLocation().ToString());
	JsonWriter->WriteValue(TEXT("title"), _TargetActor->Tilte.ToString());
	JsonWriter->WriteValue(TEXT("0"), _TargetActor->Des0.ToString());
	JsonWriter->WriteValue(TEXT("1"), _TargetActor->Des1.ToString());
	JsonWriter->WriteValue(TEXT("2"), _TargetActor->Des2.ToString());
	JsonWriter->WriteValue(TEXT("3"), _TargetActor->Des3.ToString());
	JsonWriter->WriteValue(TEXT("4"), _TargetActor->Des4.ToString());
	JsonWriter->WriteObjectEnd();
}

class AArmyPointLightActor* FJsonResourceParer::Read_PointLight(UWorld* InWorld, TSharedPtr<FJsonObject> JsonObject)
{
	if (JsonObject.IsValid())
	{
		FVector Location = FVector::ZeroVector;
		Location.InitFromString(JsonObject->GetStringField(TEXT("location")));
		float Intensity = JsonObject->GetNumberField(TEXT("intensity"));
		float Radius = JsonObject->GetNumberField(TEXT("radius"));
		FLinearColor Color;
		Color.InitFromString(JsonObject->GetStringField(TEXT("color")));
		float SourceRadius = JsonObject->GetNumberField(TEXT("sourceRadius"));
		float SourceLength = JsonObject->GetNumberField(TEXT("sourceLength"));
		bool CastShadow = JsonObject->GetIntegerField(TEXT("castShadow")) == 1;
		bool Enable = JsonObject->GetIntegerField(TEXT("enable")) == 1;
		FString LightIDStr = JsonObject->GetStringField(TEXT("lightmapid"));
		FGuid LightID;
		FGuid::Parse(LightIDStr, LightID);
		FString MobilityStr = JsonObject->GetStringField(TEXT("mobility"));

		double IndirectLightingIntensity = 1.f;
		JsonObject->TryGetNumberField(TEXT("indirectlightingintensity"), IndirectLightingIntensity);

		AArmyPointLightActor* SpawnedActor = FArmyResourceModule::Get().GetResourceManager()->AddPointLight(InWorld);
		if (SpawnedActor)
		{
			SpawnedActor->SetActorLocation(Location);
			SpawnedActor->PointLightCOM->SetIntensity(Intensity);
			SpawnedActor->PointLightCOM->SetAttenuationRadius(Radius);
			SpawnedActor->PointLightCOM->SetLightColor(Color);
			SpawnedActor->PointLightCOM->SetSourceRadius(SourceRadius);
			SpawnedActor->PointLightCOM->SetSourceLength(SourceLength);
			SpawnedActor->PointLightCOM->SetVisibility(Enable);
			SpawnedActor->PointLightCOM->IndirectLightingIntensity = IndirectLightingIntensity;
			SpawnedActor->SetActorEnableCollision(false);
			if (LightID.IsValid())
				SpawnedActor->PointLightCOM->LightGuid = LightID;
			SpawnedActor->GetRootComponent()->SetMobilityAllChildren(MobilityStr == "Movable" ? EComponentMobility::Movable : EComponentMobility::Static);
			return SpawnedActor;
		}
	}
	return nullptr;
}

class AArmySpotLightActor* FJsonResourceParer::Read_SpotLight(UWorld* InWorld, TSharedPtr<FJsonObject> JsonObject)
{
	if (JsonObject.IsValid())
	{
		FVector Location = FVector::ZeroVector;
		Location.InitFromString(JsonObject->GetStringField(TEXT("location")));
		FRotator Rotation = FRotator::ZeroRotator;
		Rotation.InitFromString(JsonObject->GetStringField(TEXT("rotation")));

		float Intensity = JsonObject->GetNumberField(TEXT("intensity"));
		float Radius = JsonObject->GetNumberField(TEXT("radius"));
		FLinearColor Color;
		Color.InitFromString(JsonObject->GetStringField(TEXT("color")));
		float SourceRadius = JsonObject->GetNumberField(TEXT("sourceRadius"));
		float SourceLength = JsonObject->GetNumberField(TEXT("sourceLength"));
		float InnerAngle = JsonObject->GetNumberField(TEXT("innerAngle"));
		float OuterAngle = JsonObject->GetNumberField(TEXT("outerAngle"));
		bool CastShadow = JsonObject->GetIntegerField(TEXT("castShadow")) == 1;
		bool Enable = JsonObject->GetIntegerField(TEXT("enable")) == 1;
		FString LightIDStr = JsonObject->GetStringField(TEXT("lightmapid"));
		FGuid LightID;
		FGuid::Parse(LightIDStr, LightID);
		FString MobilityStr = JsonObject->GetStringField(TEXT("mobility"));

		double IndirectLightingIntensity = 1.f;
		JsonObject->TryGetNumberField(TEXT("indirectlightingintensity"), IndirectLightingIntensity);

		AArmySpotLightActor* SpawnedActor = FArmyResourceModule::Get().GetResourceManager()->AddSpotLight(InWorld);
		if (SpawnedActor)
		{
			SpawnedActor->SetActorLocation(Location);
			SpawnedActor->SetActorRotation(Rotation);
			SpawnedActor->SpotLightCOM->SetIntensity(Intensity);
			SpawnedActor->SpotLightCOM->SetAttenuationRadius(Radius);
			SpawnedActor->SpotLightCOM->SetLightColor(Color);
			SpawnedActor->SpotLightCOM->SetSourceRadius(SourceRadius);
			SpawnedActor->SpotLightCOM->SetSourceLength(SourceLength);
			SpawnedActor->SpotLightCOM->SetInnerConeAngle(InnerAngle);
			SpawnedActor->SpotLightCOM->SetOuterConeAngle(OuterAngle);
			SpawnedActor->SpotLightCOM->SetVisibility(Enable);
			SpawnedActor->SpotLightCOM->IndirectLightingIntensity = IndirectLightingIntensity;
			SpawnedActor->SetActorEnableCollision(false);
			if (LightID.IsValid())
				SpawnedActor->SpotLightCOM->LightGuid = LightID;
			SpawnedActor->GetRootComponent()->SetMobilityAllChildren(MobilityStr == "Movable" ? EComponentMobility::Movable : EComponentMobility::Static);
			return SpawnedActor;
		}
	}
	return nullptr;
}

class AArmyReflectionCaptureActor* FJsonResourceParer::Read_ReflectionCapture(UWorld* InWorld, TSharedPtr<FJsonObject> JsonObject)
{
	if (JsonObject.IsValid())
	{
		FVector Location = FVector::ZeroVector;
		Location.InitFromString(JsonObject->GetStringField(TEXT("location")));
		float Radius = JsonObject->GetNumberField(TEXT("radius"));
		double Brightness = 1.f;
		JsonObject->TryGetNumberField("brightness", Brightness);

		AArmyReflectionCaptureActor* SpawnedActor = FArmyResourceModule::Get().GetResourceManager()->AddReflectionSphere(InWorld);
		if (SpawnedActor)
		{
			SpawnedActor->ReflectionCaptureCOM->SetMobility(EComponentMobility::Movable);
			SpawnedActor->SetActorLocation(Location);
			SpawnedActor->SetRadius(Radius);
			SpawnedActor->SetBrightness(Brightness);
			SpawnedActor->ReflectionCaptureCOM->SetMobility(EComponentMobility::Static);
			return SpawnedActor;
		}
	}
	return nullptr;
}

class AArmyLevelAssetBoardActor* FJsonResourceParer::Read_LevelAssetBoard(UWorld* InWorld, TSharedPtr<FJsonObject> JsonObject)
{
	if (JsonObject.IsValid())
	{
		FVector Location = FVector::ZeroVector;
		Location.InitFromString(JsonObject->GetStringField(TEXT("location")));
		FString Title = JsonObject->GetStringField(TEXT("title"));
		FString Des0 = JsonObject->GetStringField(TEXT("des0"));
		FString Des1 = JsonObject->GetStringField(TEXT("des1"));
		FString Des2 = JsonObject->GetStringField(TEXT("des2"));
		FString Des3 = JsonObject->GetStringField(TEXT("des3"));
		FString Des4 = JsonObject->GetStringField(TEXT("des4"));

		AArmyLevelAssetBoardActor* SpawnedActor = FArmyResourceModule::Get().GetResourceManager()->AddLevelAssetBoard(InWorld);
		if (SpawnedActor)
		{
			SpawnedActor->SetActorLocation(Location);
			SpawnedActor->Tilte = FText::FromString(Title);
			SpawnedActor->Des0 = FText::FromString(Des0);
			SpawnedActor->Des1 = FText::FromString(Des1);
			SpawnedActor->Des2 = FText::FromString(Des2);
			SpawnedActor->Des3 = FText::FromString(Des3);
			SpawnedActor->Des4 = FText::FromString(Des4);
			return SpawnedActor;
		}
	}
	return nullptr;
} 

