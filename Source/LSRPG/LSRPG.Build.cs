// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LSRPG : ModuleRules
{
    public LSRPG(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;


        PublicIncludePaths.AddRange(new string[] {
                "LSRPG",
                "LSRPG/AbilitySystem",
                "LSRPG/AbilitySystem/Abilities",
                "LSRPG/AbilitySystem/Attributes",
                "LSRPG/Character",
                "LSRPG/Component",
                "LSRPG/DamageActor",
                "LSRPG/Feedback",
                "LSRPG/Feedback/NumberPopComponent",
                "LSRPG/GameMode",
                "LSRPG/Item",
                "LSRPG/Player",
                "LSRPG/System",
                "LSRPG/UI",
                "LSRPG/UI/Common",
                "LSRPG/UI/Core",
                "LSRPG/UI/Damage",
                "LSRPG/UI/Equipment",
                "LSRPG/UI/Inventory",
        });

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "GameplayAbilities",
            "GameplayTasks",
            "GameplayTags",
            "Slate",
            "SlateCore",
            "HTTP",
            "JsonUtilities",
            "Json",
            "ModularGameplay",
            "UMG"

        });
    }
}
