// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SCTShapeModule : ModuleRules
{
	public SCTShapeModule(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"SCTShapeModule/Public",
				"SCTFormulaModule/Public/",
            	"SCTBaseToolModule/Public/"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"SCTShapeModule/Private"
				//"GTEngine/Public"
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"SCTBaseToolModule",
                "SCTFormulaModule"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"EngineSettings",
				"RHI",
				"RenderCore",
				"HTTP",
				"Json",
				"PakFile",
				"ProceduralMeshComponent",
				
				"SCTTess",
				"SCTXRResource",
				"SCTXRCore"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}