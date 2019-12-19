using UnrealBuildTool;

public class ArmyFrame : ModuleRules
{
	public ArmyFrame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;
        PublicDependencyModuleNames.AddRange(new string[] {
            "AppFramework",
            "Core",
            "CoreUObject",
            "InputCore",
            "Engine",
            "Slate",
            "SlateCore",
            "ArmyEditor",
            "ArmyCore",
            "ArmySlate",
			"ArmyExtrusion",
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Json",
            "HeadMountedDisplay",
            "ArmyEngine",
            "ArmyResource",
            "JsonUtilities",
            "ArmySlate",
			"JsonUtilities",
            "ArmySlate",
            "ArmyTransaction",
        });
    }
}
