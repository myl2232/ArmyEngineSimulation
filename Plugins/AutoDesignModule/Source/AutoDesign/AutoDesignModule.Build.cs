using UnrealBuildTool;
using System;
using System.IO;

public class AutoDesignModule : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }

    public AutoDesignModule(ReadOnlyTargetRules Target) : base(Target)
    {

        // For boost:: and TBB:: code

        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        // Startard Module Dependencies
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject" });
        PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Core", "Engine" });


        bool isLibrarySupported = false;


        string AutoDesignPath = Path.Combine(ThirdPartyPath, "AutoDesign");
        // Get Library Path
        string LibPath = "";
        string dllDepend = "";
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {

            isLibrarySupported = true;
        }
        else
        {
            string Err = string.Format("{0} dedicated server is made to depend on {1}. We want to avoid this, please correct module dependencies.", Target.Platform.ToString(), this.ToString()); System.Console.WriteLine(Err);
            Console.WriteLine(Err);
        }

        if (isLibrarySupported)
        {

            if (Target.Configuration == UnrealTargetConfiguration.Debug && Target.bDebugBuildsActuallyUseDebugCRT)
            {
                LibPath = Path.Combine(AutoDesignPath, "lib", "Debug");
                dllDepend = Path.Combine(AutoDesignPath, "bin", "Debug");
            }
            else
            {
                LibPath = Path.Combine(AutoDesignPath, "lib", "Release");
                dllDepend = Path.Combine(AutoDesignPath, "bin", "Release");
            }
        }

        if (isLibrarySupported)
        {
            //Add Include path            
            PublicIncludePaths.AddRange(new string[] { "Public" });

            PrivateIncludePaths.AddRange(new string[] { Path.Combine(AutoDesignPath, "include") });
            // Add Library Path
            PublicLibraryPaths.Add(LibPath);

            Console.WriteLine(string.Format("includes path:{0}", Path.Combine(AutoDesignPath, "include")));
            PublicDelayLoadDLLs.Add("AIDesign.dll");
            PublicAdditionalLibraries.Add(Path.Combine("AIDesign.lib"));
            dllDepend = Path.Combine(dllDepend, "AIDesign.dll");
            RuntimeDependencies.Add(new RuntimeDependency(dllDepend));
            Console.WriteLine("Depend {0}", dllDepend);
        }
    }
}
