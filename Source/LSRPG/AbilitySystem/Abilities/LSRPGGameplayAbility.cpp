#include "LSRPGGameplayAbility.h"

#include "LSRPGAbilitySystemComponent.h"
#include "LSRPGPlayerController.h"
#include "LSRPGCharacterBase.h"


ULSRPGGameplayAbility::ULSRPGGameplayAbility(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{

	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

}

ULSRPGAbilitySystemComponent* ULSRPGGameplayAbility::GetLSRPGAbilitySystemComponentFromActorInfo() const
{

	return (CurrentActorInfo ? Cast<ULSRPGAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

ALSRPGPlayerController* ULSRPGGameplayAbility::GetLSRPGPlayerControllerFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<ALSRPGPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
}

AController* ULSRPGGameplayAbility::GetControllerFromActorInfo() const
{

	if (CurrentActorInfo)
	{
		if (AController* PC = CurrentActorInfo->PlayerController.Get())
		{
			return PC;
		}


		// Look for a player controller or pawn in the owner chain.
		AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
		while (TestActor)
		{
			if (AController* C = Cast<AController>(TestActor))
			{
				return C;
			}

			if (APawn* Pawn = Cast<APawn>(TestActor))
			{
				return Pawn->GetController();
			}

			TestActor = TestActor->GetOwner();
		}


	}

	return nullptr;
}

ALSRPGCharacterBase* ULSRPGGameplayAbility::GetLSRPGCharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<ALSRPGCharacterBase>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}
