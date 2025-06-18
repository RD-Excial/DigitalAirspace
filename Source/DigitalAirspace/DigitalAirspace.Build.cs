// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DigitalAirspace : ModuleRules
{
	public DigitalAirspace(ReadOnlyTargetRules Target) : base(Target)
	{
        bEnableExceptions = true;       // 启用异常处理
        bUseRTTI = true;                // 启用运行时类型信息（如果需要）
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent", "WeatherSystem", "HTTP","ImportMySql", "DesktopPlatform","Sockets","Networking","Json","JsonUtilities"});

		PrivateDependencyModuleNames.AddRange(new string[] { "ApplicationCore" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
