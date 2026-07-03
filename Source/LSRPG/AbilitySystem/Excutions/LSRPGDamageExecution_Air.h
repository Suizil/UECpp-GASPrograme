#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "LSRPGDamageExecution_Air.generated.h"

class UObject;


/**
 * ULSRPGDamageExecution_Air
 *
 *	Execution used by gameplay effects to apply damage to the health attributes.
 */
UCLASS()
class ULSRPGDamageExecution_Air : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	ULSRPGDamageExecution_Air();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
