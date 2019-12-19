using UnrealBuildTool;

public class ArmyDrawCore : ModuleRules
{
    public ArmyDrawCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Engine",
                "InputCore",
                "ArmySlate"
            });

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Slate",
                "SlateCore",
                "ArmySlate",
                "ArmyCore",
                "ArmyEditor",
                "ArmyEngine",
				"ArmySceneData"
			});
    }
}