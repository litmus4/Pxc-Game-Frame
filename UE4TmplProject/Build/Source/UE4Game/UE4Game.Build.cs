// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE4Game : ModuleRules
{
    public string PxcLibPath
    {
        get
        {
            return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../../Shared/PxcLibs"));
        }
    }

    public string PxcCorePath
    {
        get
        {
            return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../PxcCore"));
        }
    }

    public string MidPath
    {
        get
        {
            return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Intermediate"));
        }
    }

	public UE4Game(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        PublicIncludePaths.Add(Path.Combine(PxcLibPath, ""));
        PublicIncludePaths.Add(Path.Combine(PxcCorePath, ""));
        string CfgStr = (Target.Configuration == UnrealTargetConfiguration.Shipping ? "Release" : "Debug");
        PublicAdditionalLibraries.Add(Path.Combine(PxcLibPath, CfgStr, "zpack.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(PxcLibPath, CfgStr, "PxcUtil.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(PxcLibPath, CfgStr, "tinyxml.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(MidPath, CfgStr + ".win32", "PublicDefinitions.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(MidPath, CfgStr + ".win32", "DataTables.lib"));
    }
}
