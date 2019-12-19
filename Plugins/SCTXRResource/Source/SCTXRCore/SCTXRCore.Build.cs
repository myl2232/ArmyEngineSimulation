using UnrealBuildTool;

public class SCTXRCore : ModuleRules
{
    public SCTXRCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "SlateCore",
            });
    }
}