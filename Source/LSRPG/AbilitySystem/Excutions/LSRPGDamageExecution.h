#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "LSRPGDamageExecution.generated.h"


class UObject;

/**
 * ULSRPGDamageExecution
 *
 *	Execution used by gameplay effects to apply damage to the health attributes.
 */

UCLASS()
class ULSRPGDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	ULSRPGDamageExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};