using UnrealBuildTool;

public class ArmySlate : ModuleRules
{
	public ArmySlate(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "ArmyCore",
            "ArmyUser"
        });

        PrivateDependencyModuleNames.AddRange(
			new string[] {
                "AppFramework",
                "Core",
				"CoreUObject",
                "Engine",
                "InputCore",
                "Json",
                "Slate",
                "SlateCore",
                "ArmyEditor",
                "ArmyEngine",
                "ArmyResource"
            }
        );
	}
}
