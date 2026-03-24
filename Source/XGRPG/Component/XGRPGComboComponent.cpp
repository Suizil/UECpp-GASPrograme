#include "XGRPGComboComponent.h"

UXGRPGComboComponent::UXGRPGComboComponent(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{	

	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

int32 UXGRPGComboComponent::GetComboIndex()
{
	return ComboIndex;
}

UAnimMontage* UXGRPGComboComponent::GetComboAnimMontage()
{
	LastPlayMontage = ComboMontages[ComboIndex];

	return ComboMontages[ComboIndex];
}

UAnimMontage* UXGRPGComboComponent::GetLastPlayAnimMontage()
{
	return LastPlayMontage;
}

void UXGRPGComboComponent::UpdateComboIndex()
{

	ComboIndex++;

	if (ComboIndex > ComboMaxIndex)
	{
		ComboIndex = 0;
	}


}

void UXGRPGComboComponent::ResetComboIndex()
{

	ComboIndex = 0;

}




void UXGRPGComboComponent::SetPressed()
{
	bShortPress = true;
}

void UXGRPGComboComponent::ResetPressed()
{
	bShortPress = false;
}

bool UXGRPGComboComponent::IsStillPressed()
{
	return bShortPress;
}

void UXGRPGComboComponent::BeginPlay()
{
	Super::BeginPlay();

	check(ComboMontages.Num()>0);

	ComboMaxIndex = ComboMontages.Num() - 1;

}
