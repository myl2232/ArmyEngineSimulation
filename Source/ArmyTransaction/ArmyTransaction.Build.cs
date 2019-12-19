using UnrealBuildTool;

public class ArmyTransaction : ModuleRules
{
    public ArmyTransaction(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
            });

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Json",
                "ArmySceneData",
                "ArmyTools"
            });
    }
}