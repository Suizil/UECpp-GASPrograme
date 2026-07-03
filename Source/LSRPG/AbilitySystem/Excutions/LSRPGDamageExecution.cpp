#include "LSRPGDamageExecution.h"
#include "AbilitySystem/Attributes/LSRPGCharacterAttributeSet.h"

struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;

	FDamageStatics()
	{
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(ULSRPGCharacterAttributeSet::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}


ULSRPGDamageExecution::ULSRPGDamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);

}

void ULSRPGDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{


#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseDamage = 0.0f;

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);

	const float DamageDone = FMath::Max(0.0f, BaseDamage);

	if (DamageDone > 0.0f)
	{
		// Apply a healing modifier, this gets turned into + health on the target
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ULSRPGCharacterAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, DamageDone));
	}



#endif // #if WITH_SERVER_CODE
}
