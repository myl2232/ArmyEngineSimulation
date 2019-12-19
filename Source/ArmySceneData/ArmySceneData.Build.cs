using UnrealBuildTool;

public class ArmySceneData : ModuleRules
{
    public ArmySceneData(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Engine",
                "ProceduralMeshComponent",
                "ArmyEngine",
                "ArmyDrawCore",
                "ArmyCore",
                "ArmyEditor",
                "ArmySlate",
				"ArmyTools",
                "Army",
				"ArmyExtrusion",
                "ArmyTransaction",
                "XRStaticLighting",
            });

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Paper2D",
				"Engine",
				"RenderCore",
                "RHI",
                "Core",
                "CoreUObject",
                "Json",
                "RenderCore",
                "Slate",
                "SlateCore",
				"ArmyTools",
                "ArmyResource",
                "ArmyFrame",
				"ArmyPanorama",
				"ArmyUser",
				"RawMesh",
                "XRStaticLighting",
				"ArmyTransaction"
            });
    }
}