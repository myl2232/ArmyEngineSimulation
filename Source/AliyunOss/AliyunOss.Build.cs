using UnrealBuildTool;

public class AliyunOss : ModuleRules
{
    public AliyunOss(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "Http",
            });

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Json",
            });
    }
}