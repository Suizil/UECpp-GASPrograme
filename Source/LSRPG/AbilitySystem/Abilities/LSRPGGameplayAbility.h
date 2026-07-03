#pragma once

#include "Abilities/GameplayAbility.h"

#include "LSRPGGameplayAbility.generated.h"


class ULSRPGAbilitySystemComponent;
class ALSRPGCharacterBase;
class ALSRPGPlayerController;

/**
 * ULSRPGGameplayAbility
 *
 *	The base gameplay ability class used by this project.
 */
UCLASS(Abstract, HideCategories = Input, Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
class LSRPG_API ULSRPGGameplayAbility : public UGameplayAbility
{

	GENERATED_BODY()

public:

	ULSRPGGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
	ULSRPGAbilitySystemComponent* GetLSRPGAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
	ALSRPGPlayerController* GetLSRPGPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
	AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
	ALSRPGCharacterBase* GetLSRPGCharacterFromActorInfo() const;


};