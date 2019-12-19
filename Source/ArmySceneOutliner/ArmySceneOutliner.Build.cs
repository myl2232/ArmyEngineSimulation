using UnrealBuildTool;

public class ArmySceneOutliner : ModuleRules
{
    public ArmySceneOutliner(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "SlateCore",
            });

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "Engine",
                "InputCore",
                "Slate",
                "SlateCore",
                "ArmyEditor",
                "ArmyCore",
                "ArmyEngine",
				"ArmyExtrusion",
                "ArmySceneData"
            });
    }
}