// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GEPTemplate : ModuleRules
{
	public GEPTemplate(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG" });

		// 게임 알림 위젯을 위해 추가함
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		PublicDefinitions.AddRange(new string[]
		{
			"_WIN32_WINNT_WIN10_TH2",
			"_WIN32_WINNT_WIN10_RS1",
			"_WIN32_WINNT_WIN10_RS2",
			"_WIN32_WINNT_WIN10_RS3",
			"_WIN32_WINNT_WIN10_RS4",
			"_WIN32_WINNT_WIN10_RS5"
		});
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
