using UnrealBuildTool;

public class ArmyTextureCompressor : ModuleRules
{
    public ArmyTextureCompressor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

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
                "CoreUObject",
                "ArmyEditor",
                "ArmyEngine",
                "ArmyDrawCore",
                "ArmyCore",
                "ArmySlate",
				"ImageCore"
			});

		AddEngineThirdPartyPrivateStaticDependencies(Target, "nvTextureTools");
	}
}