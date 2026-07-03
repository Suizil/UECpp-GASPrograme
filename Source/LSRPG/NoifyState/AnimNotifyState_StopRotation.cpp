// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_StopRotation.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UAnimNotifyState_StopRotation::UAnimNotifyState_StopRotation()
{

}

void UAnimNotifyState_StopRotation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		InCharacter->bUseControllerRotationYaw = false;
		InCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;

	}
}

void UAnimNotifyState_StopRotation::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UAnimNotifyState_StopRotation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (ACharacter* InCharacter = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		InCharacter->bUseControllerRotationYaw = false;
		InCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	}


}
