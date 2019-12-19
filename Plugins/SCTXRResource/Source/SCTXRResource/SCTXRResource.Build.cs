// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SCTXRResource : ModuleRules
{
	public SCTXRResource(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
			{
                "PakFile",
                "XmlParser",
                "SCTXRUser",
				"SCTXRCore"
            }
        );

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
                "InputCore",
				"RHI",
				"Slate",
				"SlateCore",
                "Json"				
			}
        );

		PrivateIncludePathModuleNames.AddRange(
			new string[] {
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
                "SCTXRResource/Private",
            }
		);
	}
}
