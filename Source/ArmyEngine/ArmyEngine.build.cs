using UnrealBuildTool;

public class ArmyEngine : ModuleRules
{
    public ArmyEngine(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "ArmyCore",
				"ImageCore",
				"TextureCompressor",
				"DerivedDataCache",
				"ArmyTextureCompressor"
			});

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "RenderCore",
                "RHI",
                "ArmyResource",
				"ArmyTextureCompressor"
			});
        
        PrivateIncludePaths.AddRange(
           new string[] {
                "ArmyEngine/Private",
                "ArmyEngine/Private/ThumbnailRendering"
           }
       );
    }
}