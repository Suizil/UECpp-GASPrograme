#include "XGRPGDamageExecution_Air.h"
#include "XGRPGCharacterAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"


struct FDamageAirStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageAirDef;

	FDamageAirStatics()
	{
		BaseDamageAirDef = FGameplayEffectAttributeCaptureDefinition(UXGRPGCharacterAttributeSet::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FDamageAirStatics& DamageAirStatics()
{
	static FDamageAirStatics Statics;
	return Statics;
}


UXGRPGDamageExecution_Air::UXGRPGDamageExecution_Air()
{
	RelevantAttributesToCapture.Add(DamageAirStatics().BaseDamageAirDef);
}

void UXGRPGDamageExecution_Air::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{

#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayEffectContextHandle& EffectContextHandle = Spec.GetEffectContext();

	AActor* EffectCauser = EffectContextHandle.GetEffectCauser();

	UAbilitySystemComponent* EffectASC = ExecutionParams.GetTargetAbilitySystemComponent();

	UAbilitySystemComponent* OriginalASC = ExecutionParams.GetSourceAbilitySystemComponent();

	float Defense = 0.f;
	if (const UXGRPGCharacterAttributeSet* EffectArr = 
	Cast<UXGRPGCharacterAttributeSet>(OriginalASC->GetAttributeSet(UXGRPGCharacterAttributeSet::StaticClass())))
	{
		Defense = EffectArr->GetDefense();

	}

	float Attack = 0.0f;
	if (const UXGRPGCharacterAttributeSet* Originalrr = 
	Cast<UXGRPGCharacterAttributeSet>(OriginalASC->GetAttributeSet(UXGRPGCharacterAttributeSet::StaticClass())))
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
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UXGRPGCharacterAttributeSet::GetDamageAttribute(), 
		EGameplayModOp::Additive, BaseAirDamageDone));
	}



#endif // #if WITH_SERVER_CODE
}

