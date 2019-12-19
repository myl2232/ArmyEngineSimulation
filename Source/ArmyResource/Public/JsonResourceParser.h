#pragma once

class ARMYRESOURCE_API FJsonResourceParer
{
public:
	static void Write_PointLight(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter, class AArmyPointLightActor* _TargetActor, AActor* _BaseActor);
	static void Write_SpotLight(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter, class AArmySpotLightActor* _TargetActor, AActor* _BaseActor);
	static void Write_ReflectionCapture(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter, class AArmyReflectionCaptureActor* _TargetActor, AActor* _BaseActor);
	static void Write_LevelAssetBoard(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter, class AArmyLevelAssetBoardActor* _TargetActor, AActor* _BaseActor);

	static class AArmyPointLightActor* Read_PointLight(UWorld* InWorld, TSharedPtr<FJsonObject> JsonObject);
	static class AArmySpotLightActor* Read_SpotLight(UWorld* InWorld, TSharedPtr<FJsonObject> JsonObject);
	static class AArmyReflectionCaptureActor* Read_ReflectionCapture(UWorld* InWorld, TSharedPtr<FJsonObject> JsonObject);
	static class AArmyLevelAssetBoardActor* Read_LevelAssetBoard(UWorld* InWorld, TSharedPtr<FJsonObject> JsonObject);
};