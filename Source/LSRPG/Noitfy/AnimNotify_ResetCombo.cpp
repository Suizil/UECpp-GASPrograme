#include "AnimNotify_ResetCombo.h"



#include "LSRPGCharacterBase.h"
#include "LSRPGComboComponent.h"

UAnimNotify_ResetCombo::UAnimNotify_ResetCombo()
{
}

FString UAnimNotify_ResetCombo::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_ResetCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (ALSRPGCharacterBase* LSRPGCharacterBase = Cast<ALSRPGCharacterBase>(MeshComp->GetOuter()))
	{
		LSRPGCharacterBase->GetLSRPGComboComponent()->ResetComboIndex();

	}
}
