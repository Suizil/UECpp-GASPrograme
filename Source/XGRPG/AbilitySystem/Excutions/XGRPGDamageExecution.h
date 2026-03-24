#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "XGRPGDamageExecution.generated.h"


class UObject;

/**
 * UXGRPGDamageExecution
 *
 *	Execution used by gameplay effects to apply damage to the health attributes.
 */

UCLASS()
class UXGRPGDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UXGRPGDamageExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};