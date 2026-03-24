#include "XGRPGPlayerController.h"

#include "XGRPGAbilitySystemComponent.h"
#include "XGRPGCharacterBase.h"
#include "XGRPGPlayerState.h"
#include "XGRPGHUD.h"
#include "XGRPGNumberPopComponent_UMG.h"

AXGRPGPlayerController::AXGRPGPlayerController(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{

	//伤害数字弹出组件组件
	NumberPopComponent = CreateDefaultSubobject<UXGRPGNumberPopComponent_UMG>(TEXT("NumberPopComponent"));
	NumberPopComponent->SetIsReplicated(false);

}

AXGRPGPlayerState* AXGRPGPlayerController::GetXGRPGPlayerState() const
{
	return CastChecked<AXGRPGPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UXGRPGAbilitySystemComponent* AXGRPGPlayerController::GetXGRPGAbilitySystemComponent() const
{
	AXGRPGCharacterBase* XGRPGCharacter = Cast<AXGRPGCharacterBase>(GetPawn());
	if (XGRPGCharacter)
	{
		return XGRPGCharacter->GetXGRPGAbilitySystemComponent();
	}

	return nullptr;
}

UXGRPGNumberPopComponent* AXGRPGPlayerController::GetXGRPGNumberPopComponent() const
{
	return NumberPopComponent;
}

AXGRPGHUD* AXGRPGPlayerController::GetXGRPGHUD() const
{
	return CastChecked<AXGRPGHUD>(GetHUD(), ECastCheckedType::NullAllowed);
}

void AXGRPGPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UXGRPGAbilitySystemComponent* XGRPGASC = GetXGRPGAbilitySystemComponent())
	{
		XGRPGASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}
