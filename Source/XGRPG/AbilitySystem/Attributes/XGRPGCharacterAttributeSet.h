
#pragma once

#include "CoreMinimal.h"
#include "XGRPGAttributeSet.h"

#include "XGRPGCharacterAttributeSet.generated.h"


/**
 * UXGRPGCharacterAttributeSet
 *
 *	Class that defines attributes that are necessary for taking damage.
 *	Attribute examples include: health, shields, and resistances.
 *
 *
 */
UCLASS()
class XGRPG_API UXGRPGCharacterAttributeSet : public UXGRPGAttributeSet
{
	GENERATED_BODY()

public:

	UXGRPGCharacterAttributeSet();

	ATTRIBUTE_ACCESSORS(UXGRPGCharacterAttributeSet, Damage);

	ATTRIBUTE_ACCESSORS(UXGRPGCharacterAttributeSet, Level);

	ATTRIBUTE_ACCESSORS(UXGRPGCharacterAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UXGRPGCharacterAttributeSet, MaxHealth);

	ATTRIBUTE_ACCESSORS(UXGRPGCharacterAttributeSet, Mana);
	ATTRIBUTE_ACCESSORS(UXGRPGCharacterAttributeSet, MaxMana);

	ATTRIBUTE_ACCESSORS(UXGRPGCharacterAttributeSet, Stamina);
	ATTRIBUTE_ACCESSORS(UXGRPGCharacterAttributeSet, MaxStamina);

	ATTRIBUTE_ACCESSORS(UXGRPGCharacterAttributeSet, Attack);
	ATTRIBUTE_ACCESSORS(UXGRPGCharacterAttributeSet, Defense);
	ATTRIBUTE_ACCESSORS(UXGRPGCharacterAttributeSet, Exp);
	ATTRIBUTE_ACCESSORS(UXGRPGCharacterAttributeSet, MaxExp);


	// Delegate when health changes due to damage/healing, some information may be missing on the client
	mutable FXGRPGAttributeEvent OnHealthChanged;

	// Delegate when max health changes
	mutable FXGRPGAttributeEvent OnMaxHealthChanged;

	// Delegate to broadcast when the health attribute reaches zero
	mutable FXGRPGAttributeEvent OnOutOfHealth;


	// Delegate when health changes due to damage/healing, some information may be missing on the client
	mutable FXGRPGAttributeEvent OnManaChanged;

	// Delegate when max health changes
	mutable FXGRPGAttributeEvent OnMaxManaChanged;


protected:

	UFUNCTION()
	virtual void OnRep_Damage(const FGameplayAttributeData& OldValue);


	UFUNCTION()
	virtual void OnRep_Level(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);


	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldValue);


	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Attack(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_Defense(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Exp(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_MaxExp(const FGameplayAttributeData& OldValue);



	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;


	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;


private:

	UPROPERTY(BlueprintReadOnly, Category = "XGRPG|Attribute", ReplicatedUsing = OnRep_Damage, Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

	UPROPERTY(BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_Level, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Level;

	UPROPERTY(BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_Health, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_MaxHealth, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;


	// Used to track when the health reaches 0.
	bool bOutOfHealth;

	// Store the health before any changes 
	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;





	UPROPERTY(BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_Mana, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Mana;

	UPROPERTY(BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_MaxMana, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxMana;

	UPROPERTY(BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_Stamina, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Stamina;

	UPROPERTY(BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_MaxStamina, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxStamina;

	UPROPERTY(BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_Attack, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attack;

	UPROPERTY(BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_Defense, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Defense;

	UPROPERTY(BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_Exp, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Exp;

	UPROPERTY(BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_MaxExp, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxExp;


};