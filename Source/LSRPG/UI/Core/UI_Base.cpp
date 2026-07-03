#include "UI_Base.h"

#include "LSRPGCharacterBase.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Animation/WidgetAnimation.h"
#include "GameFramework/PlayerController.h"

UUI_Base::UUI_Base(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
}

void UUI_Base::PlayWidgetAnim(const FString& InAnimName)
{
	if (UWidgetAnimation* TempAnimation = GetNameWidgetAnimation(InAnimName))
	{
		PlayAnimation(TempAnimation);
	}
}

UWidgetAnimation* UUI_Base::GetNameWidgetAnimation(const FString& InWigetName)
{
	if (UWidgetBlueprintGeneratedClass* WidgetBlueprintGenerated = Cast<UWidgetBlueprintGeneratedClass>(GetClass()))
	{
		TArray<UWidgetAnimation*> TArrayAnimations = WidgetBlueprintGenerated->Animations;

		UWidgetAnimation** MyTempAnimation = TArrayAnimations.FindByPredicate(
			[&](const UWidgetAnimation* OurAnimation) {

				return  OurAnimation->GetFName().ToString() == (InWigetName + FString("_INST"));
			});

		return  *MyTempAnimation;
	}
	return nullptr;
}

ALSRPGCharacterBase* UUI_Base::UI_GetLSRPGCharacterBase()
{
	if (GetWorld())
	{

		if (APlayerController* InPlayerController = GetWorld()->GetFirstPlayerController<APlayerController>())
		{
			if (ALSRPGCharacterBase* InCharacter = InPlayerController->GetPawn<ALSRPGCharacterBase>())
			{
				return	 InCharacter;
			}

		}

	}

	return nullptr;
}
