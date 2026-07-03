#include "AnimNotifyState_NextCombo.h"
#include "LSRPGCharacterBase.h"
#include "LSRPGCharacter.h"
#include "LSRPGComboComponent.h"

UAnimNotifyState_NextCombo::UAnimNotifyState_NextCombo()
{
}

void UAnimNotifyState_NextCombo::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp->GetOuter() && MeshComp->GetOuter()->GetWorld() && !MeshComp->GetOuter()->GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer))
	{
		if (ALSRPGCharacterBase* InCharacter = Cast<ALSRPGCharacterBase>(MeshComp->GetOuter()))
		{
			//消耗连击变量
			InCharacter->GetLSRPGComboComponent()->ResetPressed();

		}

	}


}

void UAnimNotifyState_NextCombo::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UAnimNotifyState_NextCombo::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp->GetOuter() && MeshComp->GetOuter()->GetWorld() && !MeshComp->GetOuter()->GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer))
	{

		if (ALSRPGCharacter* InCharacter = Cast<ALSRPGCharacter>(MeshComp->GetOuter()))
		{
			if (InCharacter->GetLSRPGComboComponent()->IsStillPressed())
			{
				InCharacter->ComboMelee();
			}


		}


	}

}
