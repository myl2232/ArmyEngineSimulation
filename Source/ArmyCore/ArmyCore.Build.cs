using UnrealBuildTool;

public class ArmyCore : ModuleRules
{
    public ArmyCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "SlateCore"
            });

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "ArmyTools",
            });
    }
}