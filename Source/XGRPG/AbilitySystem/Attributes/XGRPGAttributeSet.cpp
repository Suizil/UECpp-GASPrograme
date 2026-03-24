#include "XGRPGAttributeSet.h"

#include "AbilitySystem/XGRPGAbilitySystemComponent.h"
class UWorld;


UXGRPGAttributeSet::UXGRPGAttributeSet()
{
}

UWorld* UXGRPGAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UXGRPGAbilitySystemComponent* UXGRPGAttributeSet::GetXGRPGAbilitySystemComponent() const
{
	return Cast<UXGRPGAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
