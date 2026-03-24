#include "XGRPGGameplayAbility_Death.h"

#include "XGRPGGameplayAbility.h"
#include "XGRPGAbilitySystemComponent.h"
#include "XGRPGHealthComponent.h"
#include "XGRPGGameplayTag.h"
#include "XGRPGLogChannels.h"


UXGRPGGameplayAbility_Death::UXGRPGGameplayAbility_Death(const FObjectInitializer& ObjectInitializer)
{

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	bAutoStartDeath = true;

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		// Add the ability trigger tag as default to the CDO.
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = XGRPGGameplayTags::GameplayEvent_Death;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);

	}


}

void UXGRPGGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	check(ActorInfo);

	UXGRPGAbilitySystemComponent* XGRPGASC = CastChecked<UXGRPGAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	FGameplayTagContainer AbilityTypesToIgnore;
	AbilityTypesToIgnore.AddTag(XGRPGGameplayTags::Ability_Behavior_SurvivesDeath);


	// Cancel all abilities and block others from starting.
	XGRPGASC->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);

	SetCanBeCanceled(false);


	if (bAutoStartDeath)
	{
		StartDeath();
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


}

void UXGRPGGameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	check(ActorInfo);

// Always try to finish the death when the ability ends in case the ability doesn't.
// This won't do anything if the death hasn't been started.
	FinishDeath();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UXGRPGGameplayAbility_Death::StartDeath()
{
	if (UXGRPGHealthComponent* HealthComponent = UXGRPGHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == EXGRPGDeathState::NotDead)
		{
			HealthComponent->StartDeath();
		}
	}
}

void UXGRPGGameplayAbility_Death::FinishDeath()
{
	if (UXGRPGHealthComponent* HealthComponent = UXGRPGHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == EXGRPGDeathState::DeathStarted)
		{
			HealthComponent->FinishDeath();
		}
	}
}
