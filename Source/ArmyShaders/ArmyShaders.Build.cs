using UnrealBuildTool;

public class ArmyShaders : ModuleRules
{
    public ArmyShaders(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "RHI",
            "Engine",
            "RenderCore",
        });
    }
}