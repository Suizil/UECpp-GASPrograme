#include "XGRPGNumberPopActor.h"

#include "Components/WidgetComponent.h"
#include "UI_DamageNum.h"


AXGRPGDamageNumActor::AXGRPGDamageNumActor()
{
	DamageNumRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DamageNumRootComponent"));
	RootComponent = DamageNumRootComponent;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DamgeWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamgeWidget"));
	DamgeWidgetComponent->SetupAttachment(RootComponent);
	DamgeWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InitialLifeSpan = 4.0;


}

void AXGRPGDamageNumActor::UpdateNum(const FText& InText)
{
	if (DamgeWidgetComponent)
	{
		if (UUI_DamgeNumber* Damge = Cast<UUI_DamgeNumber>(DamgeWidgetComponent->GetUserWidgetObject()))
		{
			Damge->UpdateNum(InText);
		}

	}

}

void AXGRPGDamageNumActor::UpdateNumColor(const FLinearColor& InColor)
{
	if (DamgeWidgetComponent)
	{

		if (UUI_DamgeNumber* Damge = Cast<UUI_DamgeNumber>(DamgeWidgetComponent->GetUserWidgetObject()))
		{
			Damge->UpdateNumColor(InColor);
		}


	}
}
