// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;
using System;

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
        string CfgStrOut = "Release";
        string CfgStr = (Target.Configuration == UnrealTargetConfiguration.DebugGame ? "Debug" : "Release");
        if (Target.Platform == UnrealTargetPlatform.Win64/* ||*/)
        {
            PublicAdditionalLibraries.Add(Path.Combine(PxcLibPath, CfgStrOut, "zpack64.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(PxcLibPath, CfgStrOut, "PxcUtil64.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(PxcLibPath, CfgStrOut, "tinyxml64.lib"));
        }
        else
        {
            PublicAdditionalLibraries.Add(Path.Combine(PxcLibPath, CfgStrOut, "zpack.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(PxcLibPath, CfgStrOut, "PxcUtil.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(PxcLibPath, CfgStrOut, "tinyxml.lib"));
        }
        //FLAGJK 存在两个编译问题之一：PxcCore连接还是未找到外部符号
        PublicAdditionalLibraries.Add(Path.Combine(MidPath, CfgStr + ".win32", "PublicDefinitions.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(MidPath, CfgStr + ".win32", "DataTables.lib"));
    }
}
