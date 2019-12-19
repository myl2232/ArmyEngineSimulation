// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ArmyEditor : ModuleRules
{
	public ArmyEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;
        PublicDependencyModuleNames.AddRange(
            new string[]
			{
                "Engine"
			}
        );

		PrivateDependencyModuleNames.AddRange(
			new string[] {
                "ApplicationCore",
                "Core",
				"CoreUObject",
                "ImageDownload",
                "InputCore",
                "RHI",
                "Slate",
                "SlateCore",
                "RenderCore",
                "ArmyEngine",
                "ArmyResource",
                "ArmyShaders",
                "ArmySlate",
				"ArmyExtrusion",
                "ArmyCore",
				"ArmySceneData",
            }
        );

		PrivateIncludePathModuleNames.AddRange(
			new string[] {
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
            }
        );
	}
}
