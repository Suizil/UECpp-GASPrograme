#include "LSRPGGameplayAbility_Death.h"

#include "LSRPGGameplayAbility.h"
#include "LSRPGAbilitySystemComponent.h"
#include "LSRPGHealthComponent.h"
#include "LSRPGGameplayTag.h"
#include "LSRPGLogChannels.h"


ULSRPGGameplayAbility_Death::ULSRPGGameplayAbility_Death(const FObjectInitializer& ObjectInitializer)
{

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	bAutoStartDeath = true;

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		// Add the ability trigger tag as default to the CDO.
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = LSRPGGameplayTags::GameplayEvent_Death;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);

	}


}

void ULSRPGGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	check(ActorInfo);

	ULSRPGAbilitySystemComponent* LSRPGASC = CastChecked<ULSRPGAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	FGameplayTagContainer AbilityTypesToIgnore;
	AbilityTypesToIgnore.AddTag(LSRPGGameplayTags::Ability_Behavior_SurvivesDeath);


	// Cancel all abilities and block others from starting.
	LSRPGASC->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);

	SetCanBeCanceled(false);


	if (bAutoStartDeath)
	{
		StartDeath();
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


}

void ULSRPGGameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	check(ActorInfo);

// Always try to finish the death when the ability ends in case the ability doesn't.
// This won't do anything if the death hasn't been started.
	FinishDeath();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void ULSRPGGameplayAbility_Death::StartDeath()
{
	if (ULSRPGHealthComponent* HealthComponent = ULSRPGHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == ELSRPGDeathState::NotDead)
		{
			HealthComponent->StartDeath();
		}
	}
}

void ULSRPGGameplayAbility_Death::FinishDeath()
{
	if (ULSRPGHealthComponent* HealthComponent = ULSRPGHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == ELSRPGDeathState::DeathStarted)
		{
			HealthComponent->FinishDeath();
		}
	}
}
