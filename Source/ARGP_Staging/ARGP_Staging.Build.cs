// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ARGP_Staging : ModuleRules
{
	public ARGP_Staging(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "UMG", "SlateCore", "InputCore", "MoviePlayer", "GameplayAbilities", "GameplayTags", "AIModule", "GameplayTasks", "DlgSystem", "QuestSystem"});

    }
}
