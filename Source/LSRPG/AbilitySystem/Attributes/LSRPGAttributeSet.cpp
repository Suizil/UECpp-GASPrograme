#include "LSRPGAttributeSet.h"

#include "LSRPGAbilitySystemComponent.h"
class UWorld;


ULSRPGAttributeSet::ULSRPGAttributeSet()
{
}

UWorld* ULSRPGAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

ULSRPGAbilitySystemComponent* ULSRPGAttributeSet::GetLSRPGAbilitySystemComponent() const
{
	return Cast<ULSRPGAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
