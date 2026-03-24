// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class XGRPG : ModuleRules
{
    public XGRPG(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        OptimizeCode = CodeOptimization.Never;


        PublicIncludePaths.AddRange(new string[] {
                "XGRPG",
                "XGRPG/AbilitySystem",
                "XGRPG/AbilitySystem/Abilities",
                "XGRPG/AbilitySystem/Attributes",
                "XGRPG/Character",
                "XGRPG/Component",
                "XGRPG/DamageActor",
                "XGRPG/Feedback",
                "XGRPG/Feedback/NumberPopComponent",
                "XGRPG/GameMode",
                "XGRPG/Item",
                "XGRPG/Player",
                "XGRPG/System",
                "XGRPG/UI",
                "XGRPG/UI/Common",
                "XGRPG/UI/Core",
                "XGRPG/UI/Damage",
                "XGRPG/UI/Equipment",
                "XGRPG/UI/Inventory",
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
