// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;
using System;

public class UEGame : ModuleRules
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

	public UEGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "DeveloperSettings" });
        PublicDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTags", "GameplayTasks" });

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "MotionTrajectory" });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        bUseRTTI = true;
        PublicIncludePaths.Add(Path.Combine(PxcLibPath, ""));
        PublicIncludePaths.Add(Path.Combine(PxcCorePath, ""));
        string CfgStrOut = "Release";
        //TODOJK 尽量不要牺牲调试体验，回头还得看看Debug编译连接时的配置不匹配
        string CfgStr = "Release";//(Target.Configuration == UnrealTargetConfiguration.DebugGame ? "Debug" : "Release");
        string PlatSufStr = "";
        if (Target.Platform == UnrealTargetPlatform.Win64/* ||*/)
        {
            PublicAdditionalLibraries.Add(Path.Combine(PxcLibPath, CfgStrOut, "zpack64.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(PxcLibPath, CfgStrOut, "PxcUtil64.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(PxcLibPath, CfgStrOut, "tinyxml64.lib"));
            PlatSufStr = ".win64";
        }
        else
        {
            PublicAdditionalLibraries.Add(Path.Combine(PxcLibPath, CfgStrOut, "zpack.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(PxcLibPath, CfgStrOut, "PxcUtil.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(PxcLibPath, CfgStrOut, "tinyxml.lib"));
            PlatSufStr = ".win32";
        }
        PublicAdditionalLibraries.Add(Path.Combine(MidPath, CfgStr + PlatSufStr, "PublicDefinitions.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(MidPath, CfgStr + PlatSufStr, "DataTables.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(MidPath, CfgStr + PlatSufStr, "MonoControl.lib"));
    }
}
