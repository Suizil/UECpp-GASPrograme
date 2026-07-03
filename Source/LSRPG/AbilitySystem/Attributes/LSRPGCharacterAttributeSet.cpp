#include "LSRPGCharacterAttributeSet.h"


#include "Net/UnrealNetwork.h"
#include "AttributeSet.h"
#include "GameplayEffectExtension.h"


#include "AbilitySystemComponent.h"
#include "LSRPGAbilitySystemComponent.h"


ULSRPGCharacterAttributeSet::ULSRPGCharacterAttributeSet()
	: Health(100.0f)
	, MaxHealth(100.0f)
	, Mana(80.0f)
	, MaxMana(80.0f)
	, Stamina(50.0F)
	, MaxStamina(50.0f)
	, Attack(10.f)
	, Defense(2.0f)
	, Exp(0.0),
	MaxExp(120.f)
{

	bOutOfHealth = false;

	MaxHealthBeforeAttributeChange = 0.0f;
	HealthBeforeAttributeChange = 0.0f;


}

void ULSRPGCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ULSRPGCharacterAttributeSet, Damage, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(ULSRPGCharacterAttributeSet, Level, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(ULSRPGCharacterAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULSRPGCharacterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(ULSRPGCharacterAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULSRPGCharacterAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(ULSRPGCharacterAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULSRPGCharacterAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(ULSRPGCharacterAttributeSet, Attack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULSRPGCharacterAttributeSet, Defense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULSRPGCharacterAttributeSet, Exp, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULSRPGCharacterAttributeSet, MaxExp, COND_None, REPNOTIFY_Always);

}

bool ULSRPGCharacterAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	Super::PreGameplayEffectExecute(Data);

	// Save the current health
	HealthBeforeAttributeChange = GetHealth();
	MaxHealthBeforeAttributeChange = GetMaxHealth();



	return true;
}

void ULSRPGCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumHealth = 0.0f;


	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();

	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Convert into -Health and then clamp
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		SetDamage(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Clamp and fall into out of health handling below
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		// TODO clamp current health?



		// Notify on any requested max health changes
		OnMaxHealthChanged.Broadcast(Instigator, 
		Causer,
		&Data.EffectSpec,
		Data.EvaluatedData.Magnitude, 
		MaxHealthBeforeAttributeChange, 
		GetMaxHealth());
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{

		// Clamp and fall into out of health handling below
		SetMana(FMath::Clamp(GetMana(), 0, GetMaxMana()));

	}
	else if (Data.EvaluatedData.Attribute == GetMaxManaAttribute())
	{
		// TODO clamp current health?

		// Notify on any requested max health changes
		OnMaxManaChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxHealthBeforeAttributeChange, GetMaxHealth());
	}

	// If health has actually changed activate callbacks
	if (GetHealth() != HealthBeforeAttributeChange)
	{
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	// Check health again in case an event above changed it.
	bOutOfHealth = (GetHealth() <= 0.0f);


}

void ULSRPGCharacterAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	return Super::PreAttributeBaseChange(Attribute, NewValue);
}

void ULSRPGCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	return Super::PreAttributeChange(Attribute, NewValue);
}

void ULSRPGCharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		// Make sure current health is not greater than the new max health.
		if (GetHealth() > NewValue)
		{
			ULSRPGAbilitySystemComponent* LSRPGASC = GetLSRPGAbilitySystemComponent();
			check(LSRPGASC);

			LSRPGASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	if (bOutOfHealth && (GetHealth() > 0.0f))
	{
		bOutOfHealth = false;
	}




}

void ULSRPGCharacterAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{

	if (Attribute == GetHealthAttribute())
	{
		// Do not allow health to go negative or above max health.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// Do not allow max health to drop below 1.
		NewValue = FMath::Max(NewValue, 1.0f);
	}

}

void ULSRPGCharacterAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULSRPGCharacterAttributeSet, Damage, OldValue);

}


void ULSRPGCharacterAttributeSet::OnRep_Level(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULSRPGCharacterAttributeSet, Level, OldValue);
}

void ULSRPGCharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULSRPGCharacterAttributeSet, Health, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	// These events on the client should not be changing attributes

	const float CurrentHealth = GetHealth();
	const float EstimatedMagnitude = CurrentHealth - OldValue.GetCurrentValue();

	OnHealthChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);

	if (!bOutOfHealth && CurrentHealth <= 0.0f)
	{
		OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);
	}

	bOutOfHealth = (CurrentHealth <= 0.0f);

}

void ULSRPGCharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULSRPGCharacterAttributeSet, MaxHealth, OldValue);

	// Call the change callback, but without an instigator
	//This could be changed to an explicit RPC in the future
	OnMaxHealthChanged.Broadcast(nullptr, nullptr, nullptr, GetMaxHealth() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMaxHealth());

}

void ULSRPGCharacterAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULSRPGCharacterAttributeSet, Mana, OldValue);

	const float CurrentMana = GetMana();
	const float EstimatedMagnitude = CurrentMana - OldValue.GetCurrentValue();

	OnManaChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentMana);
}

void ULSRPGCharacterAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULSRPGCharacterAttributeSet, MaxMana, OldValue);

	OnMaxManaChanged.Broadcast(nullptr, nullptr, nullptr, GetMaxMana() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMaxMana());
}

void ULSRPGCharacterAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULSRPGCharacterAttributeSet, Stamina, OldValue);
}

void ULSRPGCharacterAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULSRPGCharacterAttributeSet, MaxStamina, OldValue);
}

void ULSRPGCharacterAttributeSet::OnRep_Attack(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULSRPGCharacterAttributeSet, Attack, OldValue);
}

void ULSRPGCharacterAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULSRPGCharacterAttributeSet, Defense, OldValue);
}

void ULSRPGCharacterAttributeSet::OnRep_Exp(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULSRPGCharacterAttributeSet, Exp, OldValue);
}

void ULSRPGCharacterAttributeSet::OnRep_MaxExp(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULSRPGCharacterAttributeSet, MaxExp, OldValue);
}
