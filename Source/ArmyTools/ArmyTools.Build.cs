using UnrealBuildTool;

public class ArmyTools : ModuleRules
{
    public ArmyTools(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Engine",
                "Slate",
                "SlateCore",
                "InputCore"
            });

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "Army",
                "CoreUObject",
                "ArmyEditor",
                "ArmyEngine",
                "ArmyDrawCore",
                "ArmyCore",
                "ArmySlate"
            });
    }
}