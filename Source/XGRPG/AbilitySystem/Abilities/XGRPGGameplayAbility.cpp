#include "XGRPGGameplayAbility.h"

#include "XGRPGAbilitySystemComponent.h"
#include "XGRPGPlayerController.h"
#include "XGRPGCharacterBase.h"


UXGRPGGameplayAbility::UXGRPGGameplayAbility(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{

	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

}

UXGRPGAbilitySystemComponent* UXGRPGGameplayAbility::GetXGRPGAbilitySystemComponentFromActorInfo() const
{

	return (CurrentActorInfo ? Cast<UXGRPGAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

AXGRPGPlayerController* UXGRPGGameplayAbility::GetXGRPGPlayerControllerFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AXGRPGPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
}

AController* UXGRPGGameplayAbility::GetControllerFromActorInfo() const
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

AXGRPGCharacterBase* UXGRPGGameplayAbility::GetXGRPGCharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AXGRPGCharacterBase>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}
