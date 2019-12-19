using UnrealBuildTool;

public class SCTXRUser : ModuleRules
{
    public SCTXRUser(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "HTTP",
            });

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "SCTXRCore",
                "Json",
                "Engine",
            });
    }
}