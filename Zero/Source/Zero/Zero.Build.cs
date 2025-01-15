// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;


public class Zero : ModuleRules
{
	public Zero(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "MotionWarping", "AnimGraphRuntime", "UMG", "LevelSequence", "MovieScene", "TemplateSequence", "AIModule", "NavigationSystem", "RenderCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        PublicIncludePaths.AddRange(new string[] {
            Path.Combine(ModuleDirectory, "AI"),
            Path.Combine(ModuleDirectory, "AnimNotifies"),
            Path.Combine(ModuleDirectory, "Characters"),
            Path.Combine(ModuleDirectory, "Components"),
            Path.Combine(ModuleDirectory, "Controllers"),
            Path.Combine(ModuleDirectory, "GameInstances"),
            Path.Combine(ModuleDirectory, "GameModes"),
            Path.Combine(ModuleDirectory, "Interactables"),
            Path.Combine(ModuleDirectory, "Interfaces"),
            Path.Combine(ModuleDirectory, "Items"),
            Path.Combine(ModuleDirectory, "Managers"),
            Path.Combine(ModuleDirectory, "PooledActors"),
            Path.Combine(ModuleDirectory, "UI"),
            Path.Combine(ModuleDirectory, "Utilities"),
            Path.Combine(ModuleDirectory, "Weapons"),
        });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
