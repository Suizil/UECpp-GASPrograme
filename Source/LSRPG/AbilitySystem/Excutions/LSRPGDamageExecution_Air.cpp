#include "LSRPGDamageExecution_Air.h"
#include "LSRPGCharacterAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"


struct FDamageAirStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageAirDef;

	FDamageAirStatics()
	{
		BaseDamageAirDef = FGameplayEffectAttributeCaptureDefinition(ULSRPGCharacterAttributeSet::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FDamageAirStatics& DamageAirStatics()
{
	static FDamageAirStatics Statics;
	return Statics;
}


ULSRPGDamageExecution_Air::ULSRPGDamageExecution_Air()
{
	RelevantAttributesToCapture.Add(DamageAirStatics().BaseDamageAirDef);
}

void ULSRPGDamageExecution_Air::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{

#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayEffectContextHandle& EffectContextHandle = Spec.GetEffectContext();

	AActor* EffectCauser = EffectContextHandle.GetEffectCauser();

	UAbilitySystemComponent* EffectASC = ExecutionParams.GetTargetAbilitySystemComponent();

	UAbilitySystemComponent* OriginalASC = ExecutionParams.GetSourceAbilitySystemComponent();

	float Defense = 0.f;
	if (const ULSRPGCharacterAttributeSet* EffectArr =
	Cast<ULSRPGCharacterAttributeSet>(OriginalASC->GetAttributeSet(ULSRPGCharacterAttributeSet::StaticClass())))
	{
		Defense = EffectArr->GetDefense();

	}

	float Attack = 0.0f;
	if (const ULSRPGCharacterAttributeSet* Originalrr =
	Cast<ULSRPGCharacterAttributeSet>(OriginalASC->GetAttributeSet(ULSRPGCharacterAttributeSet::StaticClass())))
	{
		Attack = Originalrr->GetAttack();

	}


	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;


	float BaseAirDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageAirStatics().BaseDamageAirDef, EvaluateParameters, BaseAirDamage);

	const float BaseAirDamageDone = FMath::Max(0.0f, BaseAirDamage + Attack - Defense);

	if (BaseAirDamage > 0.0f)
	{
		// Apply a healing modifier, this gets turned into + health on the target
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ULSRPGCharacterAttributeSet::GetDamageAttribute(),
		EGameplayModOp::Additive, BaseAirDamageDone));
	}



#endif // #if WITH_SERVER_CODE
}

