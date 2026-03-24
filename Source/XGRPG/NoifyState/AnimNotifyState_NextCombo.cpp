#include "AnimNotifyState_NextCombo.h"
#include "XGRPGCharacterBase.h"
#include "XGRPGCharacter.h"
#include "XGRPGComboComponent.h"

UAnimNotifyState_NextCombo::UAnimNotifyState_NextCombo()
{
}

void UAnimNotifyState_NextCombo::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp->GetOuter() && MeshComp->GetOuter()->GetWorld() && !MeshComp->GetOuter()->GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer))
	{
		if (AXGRPGCharacterBase* InCharacter = Cast<AXGRPGCharacterBase>(MeshComp->GetOuter()))
		{
			//消耗连击变量
			InCharacter->GetXGRPGComboComponent()->ResetPressed();

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

		if (AXGRPGCharacter* InCharacter = Cast<AXGRPGCharacter>(MeshComp->GetOuter()))
		{
			if (InCharacter->GetXGRPGComboComponent()->IsStillPressed())
			{
				InCharacter->ComboMelee();
			}


		}


	}

}
