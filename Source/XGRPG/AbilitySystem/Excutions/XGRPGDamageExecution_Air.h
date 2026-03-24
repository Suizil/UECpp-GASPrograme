#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "XGRPGDamageExecution_Air.generated.h"

class UObject;


/**
 * UXGRPGDamageExecution_Air
 *
 *	Execution used by gameplay effects to apply damage to the health attributes.
 */
UCLASS()
class UXGRPGDamageExecution_Air : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UXGRPGDamageExecution_Air();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
