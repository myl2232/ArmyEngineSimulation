using UnrealBuildTool;

public class Army : ModuleRules
{
	public Army(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;

        DynamicallyLoadedModuleNames.AddRange(
        new string[] {
             "DesktopPlatform",
        });


        PublicDependencyModuleNames.AddRange(new string[] {
            "AliyunOss",
            "AppFramework",
            "Core",
            "CoreUObject",
            "InputCore",
            "Engine",
            "Slate",
            "SlateCore",
            "ArmyCameraParam",
            "ArmyDrawCore",
            "ArmyEditor",
            "ArmySceneOutliner",
            "ArmyCore",
            "ArmySlate",
            "ArmySceneData",
            "ArmyTools",
            "ArmyFrame",
            "ArmyPanorama",
			"ArmyExtrusion",
            "ApplicationCore",
			"ArmySceneData",
            "ArmyResource",
            "Json",
            "SCTShapeModule",
            "AutoDesignModule",
            "SCTFormulaModule"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "SteamVR",
            "HeadMountedDisplay",
            "ArmyEngine",
            "RenderCore",
            "ArmyUser",
            "JsonUtilities",
            "ArmySlate",
            "ArmyTransaction",
            "ArmyCameraParam",
            "ZipUtility",
            "SCTBaseToolModule",
			"SCTXRResource",
            "TargetPlatform",
            "RawMesh",
            "XRStaticLighting",
        });
    }
};