#pragma once

#include "AbilitySystemComponent.h"

#include "XGRPGAbilitySystemComponent.generated.h"

UCLASS()
class XGRPG_API UXGRPGAbilitySystemComponent : public UAbilitySystemComponent
{

	GENERATED_BODY()

public:

	UXGRPGAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	void AbilityInputTagPressed(const FGameplayTag& InputTag);

	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	void ClearAbilityInput();

protected:

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;




protected:

	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;


};