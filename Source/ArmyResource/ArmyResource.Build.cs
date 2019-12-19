// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ArmyResource : ModuleRules
{
	public ArmyResource(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;
        PublicDependencyModuleNames.AddRange(
            new string[]
			{
                "Engine",
                "PakFile",
                "XmlParser",
                "ArmyCore",
                "ArmyUser",
            }
        );

		PrivateDependencyModuleNames.AddRange(
			new string[] {
                "Army",
				"Core",
				"CoreUObject",
                "InputCore",
				"RHI",
                "RenderCore",
				"Slate",
				"SlateCore",
                "Json",
                "ArmyEngine",
				"ArmySceneData"
			}
        );

		PrivateIncludePathModuleNames.AddRange(
			new string[] {
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
                "ArmyResource/Private",
            }
		);
	}
}
