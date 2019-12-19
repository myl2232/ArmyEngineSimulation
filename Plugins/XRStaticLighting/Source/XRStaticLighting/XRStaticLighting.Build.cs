// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class XRStaticLighting : ModuleRules
	{
		public XRStaticLighting(ReadOnlyTargetRules Target) : base(Target)
		{
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.AddRange(
				new string[] {
                    "Programs/UnrealLightmass/Public"
                }
				);

			PrivateIncludePaths.AddRange(
				new string[] {
                    //"MetisPlugin/Private",
					// ... add other private include paths required here ...
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
                    "Engine",
                    "Core",
                    "CoreUObject",
					// ... add other public dependencies that you statically link with here ...
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
                    "Slate",
                    "SlateCore",
                    "SwarmInterface",
                    "Landscape",
                    "RHI",
                    "RenderCore",
                    "RawMesh",
                    "ShaderFormatD3D",

            // ... add private dependencies that you statically link with here ...
                });

            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                DynamicallyLoadedModuleNames.AddRange(
                  new string[]
                  {
                        "TargetPlatform",
                        "WindowsTargetPlatform",
					    "TextureFormatDXT",
                        "TextureFormatUncompressed",
                // ... add any modules that your module loads dynamically here ...
            }
                );
            }
                  
		}
	}
}
