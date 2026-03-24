#pragma once

#include "Abilities/GameplayAbility.h"

#include "XGRPGGameplayAbility.generated.h"


class UXGRPGAbilitySystemComponent;
class AXGRPGCharacterBase;
class AXGRPGPlayerController;

/**
 * UXGRPGGameplayAbility
 *
 *	The base gameplay ability class used by this project.
 */
UCLASS(Abstract, HideCategories = Input, Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
class XGRPG_API UXGRPGGameplayAbility : public UGameplayAbility
{

	GENERATED_BODY()

public:

	UXGRPGGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
	UXGRPGAbilitySystemComponent* GetXGRPGAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
	AXGRPGPlayerController* GetXGRPGPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
	AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
	AXGRPGCharacterBase* GetXGRPGCharacterFromActorInfo() const;


};