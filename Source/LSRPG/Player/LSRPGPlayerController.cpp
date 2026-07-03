#include "LSRPGPlayerController.h"

#include "LSRPGAbilitySystemComponent.h"
#include "LSRPGCharacterBase.h"
#include "LSRPGPlayerState.h"
#include "LSRPGHUD.h"
#include "LSRPGNumberPopComponent_UMG.h"

ALSRPGPlayerController::ALSRPGPlayerController(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{

	//伤害数字弹出组件组件
	NumberPopComponent = CreateDefaultSubobject<ULSRPGNumberPopComponent_UMG>(TEXT("NumberPopComponent"));
	NumberPopComponent->SetIsReplicated(false);

}

ALSRPGPlayerState* ALSRPGPlayerController::GetLSRPGPlayerState() const
{
	return CastChecked<ALSRPGPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

ULSRPGAbilitySystemComponent* ALSRPGPlayerController::GetLSRPGAbilitySystemComponent() const
{
	ALSRPGCharacterBase* LSRPGCharacter = Cast<ALSRPGCharacterBase>(GetPawn());
	if (LSRPGCharacter)
	{
		return LSRPGCharacter->GetLSRPGAbilitySystemComponent();
	}

	return nullptr;
}

ULSRPGNumberPopComponent* ALSRPGPlayerController::GetLSRPGNumberPopComponent() const
{
	return NumberPopComponent;
}

ALSRPGHUD* ALSRPGPlayerController::GetLSRPGHUD() const
{
	return CastChecked<ALSRPGHUD>(GetHUD(), ECastCheckedType::NullAllowed);
}

void ALSRPGPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (ULSRPGAbilitySystemComponent* LSRPGASC = GetLSRPGAbilitySystemComponent())
	{
		LSRPGASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}
