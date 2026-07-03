#include "LSRPGComboComponent.h"

ULSRPGComboComponent::ULSRPGComboComponent(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{	

	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

int32 ULSRPGComboComponent::GetComboIndex()
{
	return ComboIndex;
}

UAnimMontage* ULSRPGComboComponent::GetComboAnimMontage()
{
	LastPlayMontage = ComboMontages[ComboIndex];

	return ComboMontages[ComboIndex];
}

UAnimMontage* ULSRPGComboComponent::GetLastPlayAnimMontage()
{
	return LastPlayMontage;
}

void ULSRPGComboComponent::UpdateComboIndex()
{

	ComboIndex++;

	if (ComboIndex > ComboMaxIndex)
	{
		ComboIndex = 0;
	}


}

void ULSRPGComboComponent::ResetComboIndex()
{

	ComboIndex = 0;

}




void ULSRPGComboComponent::SetPressed()
{
	bShortPress = true;
}

void ULSRPGComboComponent::ResetPressed()
{
	bShortPress = false;
}

bool ULSRPGComboComponent::IsStillPressed()
{
	return bShortPress;
}

void ULSRPGComboComponent::BeginPlay()
{
	Super::BeginPlay();

	check(ComboMontages.Num()>0);

	ComboMaxIndex = ComboMontages.Num() - 1;

}
