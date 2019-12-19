// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ArmyExtrusion : ModuleRules
{
	public ArmyExtrusion(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;
        PublicDependencyModuleNames.AddRange(new string[] {
        });

        PrivateDependencyModuleNames.AddRange(
           new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "ArmyEngine",
                "RHI",
                "RenderCore",
                "ArmyCore",
                "ArmySceneData",
                "ArmyDrawCore",
				"ArmyTools"
           }
       );

        PrivateIncludePaths.AddRange(
            new string[]
            {
            });
    }
}
