// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ArmyPanorama : ModuleRules
{
	public ArmyPanorama(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "ImageWrapper",
                "RenderCore",
                "Slate",
                "SlateCore",
                "ImageDownload",
        });

        PrivateDependencyModuleNames.AddRange(
           new string[] {
                "Army",
                "XmlParser",
                "Json",
                "ArmyEngine",
                "ArmyEditor",
                "ArmyCore",
                "ArmySlate",
                "ApplicationCore",
                "ArmySceneData",
           }
       );
    }
}
