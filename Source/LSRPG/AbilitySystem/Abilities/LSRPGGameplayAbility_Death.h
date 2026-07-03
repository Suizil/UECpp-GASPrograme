#pragma once

#include "LSRPGGameplayAbility.h"

#include "LSRPGGameplayAbility_Death.generated.h"


class UObject;
struct FFrame;
struct FGameplayAbilityActorInfo;
struct FGameplayEventData;


/**
 * ULSRPGGameplayAbility_Death
 *
 *	Gameplay ability used for handling death.
 *	Ability is activated automatically via the "GameplayEvent.Death" ability trigger tag.
 * 
 */

UCLASS(Abstract)
class ULSRPGGameplayAbility_Death : public ULSRPGGameplayAbility
{
	GENERATED_BODY()



public:

	ULSRPGGameplayAbility_Death(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData) override;
	
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	bool bReplicateEndAbility, 
	bool bWasCancelled) override;


	// Starts the death sequence.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
	void StartDeath();

	// Finishes the death sequence.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
	void FinishDeath();


protected:

	// If enabled, the ability will automatically call StartDeath.  FinishDeath is always called when the ability ends if the death was started.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Death")
	bool bAutoStartDeath;


};