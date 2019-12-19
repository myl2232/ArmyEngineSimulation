using UnrealBuildTool;


public class ArmyCameraParam : ModuleRules
{
    public ArmyCameraParam(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;
        PublicDependencyModuleNames.AddRange(
            new string[] {
                
            });

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Army",
                "Core",
                "CoreUObject",
                "Engine",
                "RenderCore",
                "RHI",
                "Slate",
                "SlateCore",
                "ArmySceneData",
                "ArmyFrame",
            });
    }
}