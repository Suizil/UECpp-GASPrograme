#include "AnimNotify_ResetCombo.h"



#include "XGRPGCharacterBase.h"
#include "XGRPGComboComponent.h"

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

	if (AXGRPGCharacterBase* XGRPGCharacterBase = Cast<AXGRPGCharacterBase>(MeshComp->GetOuter()))
	{
		XGRPGCharacterBase->GetXGRPGComboComponent()->ResetComboIndex();

	}
}
