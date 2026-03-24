#include "XGRPGGameplayAbility_Jump.h"

#include "XGRPGCharacterBase.h"

UXGRPGGameplayAbility_Jump::UXGRPGGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

}

bool UXGRPGGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const AXGRPGCharacterBase* XGRPGCharacter = Cast<AXGRPGCharacterBase>(ActorInfo->AvatarActor.Get());

	if (!XGRPGCharacter || !XGRPGCharacter->CanJump())
	{
		return false;

	}


	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}


	return true;
}

void UXGRPGGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{

	// Stop jumping in case the ability blueprint doesn't call it.
	CharacterJumpStop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void UXGRPGGameplayAbility_Jump::CharacterJumpStart()
{

	if (AXGRPGCharacterBase* XGRPGCharacter = GetXGRPGCharacterFromActorInfo())
	{
		if (XGRPGCharacter->IsLocallyControlled() && !XGRPGCharacter->bPressedJump)
		{
			XGRPGCharacter->UnCrouch();
			XGRPGCharacter->Jump();
		}
	}
}

void UXGRPGGameplayAbility_Jump::CharacterJumpStop()
{
	if (AXGRPGCharacterBase* XGRPGCharacter = GetXGRPGCharacterFromActorInfo())
	{
		if (XGRPGCharacter->IsLocallyControlled() && XGRPGCharacter->bPressedJump)
		{
			XGRPGCharacter->StopJumping();
		}
	}

}
