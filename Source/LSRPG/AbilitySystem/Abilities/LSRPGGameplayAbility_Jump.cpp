#include "LSRPGGameplayAbility_Jump.h"

#include "LSRPGCharacterBase.h"

ULSRPGGameplayAbility_Jump::ULSRPGGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

}

bool ULSRPGGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const ALSRPGCharacterBase* LSRPGCharacter = Cast<ALSRPGCharacterBase>(ActorInfo->AvatarActor.Get());

	if (!LSRPGCharacter || !LSRPGCharacter->CanJump())
	{
		return false;

	}


	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}


	return true;
}

void ULSRPGGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{

	// Stop jumping in case the ability blueprint doesn't call it.
	CharacterJumpStop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void ULSRPGGameplayAbility_Jump::CharacterJumpStart()
{

	if (ALSRPGCharacterBase* LSRPGCharacter = GetLSRPGCharacterFromActorInfo())
	{
		if (LSRPGCharacter->IsLocallyControlled() && !LSRPGCharacter->bPressedJump)
		{
			LSRPGCharacter->UnCrouch();
			LSRPGCharacter->Jump();
		}
	}
}

void ULSRPGGameplayAbility_Jump::CharacterJumpStop()
{
	if (ALSRPGCharacterBase* LSRPGCharacter = GetLSRPGCharacterFromActorInfo())
	{
		if (LSRPGCharacter->IsLocallyControlled() && LSRPGCharacter->bPressedJump)
		{
			LSRPGCharacter->StopJumping();
		}
	}

}
