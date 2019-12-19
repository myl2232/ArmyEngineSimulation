using UnrealBuildTool;

public class ArmyUser : ModuleRules
{
    public ArmyUser(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Army"
            });

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "ArmyCore",
                "Engine",
                "Json",
                "Army",
                "ArmyFrame",
                "Slate",
                "SlateCore",
                "ArmySlate"
            });
    }
}