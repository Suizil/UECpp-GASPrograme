#pragma once

#include "CoreMinimal.h"

#include "LSRPGComponentBase.h"

#include "NativeGameplayTags.h"

#include "LSRPGHealthComponent.generated.h"


class ULSRPGHealthComponent;

class ULSRPGAbilitySystemComponent;
class ULSRPGCharacterAttributeSet;

class UObject;
struct FFrame;
struct FGameplayEffectSpec;




DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLSRPGHealth_DeathEvent, AActor*, OwningActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FLSRPGHealth_AttributeChanged,
ULSRPGHealthComponent*,HealthComponent,
float, OldValue,
float, NewValue,
AActor*, Instigator);


UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_FellOutOfWorld);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Lyra_Damage_Message);



/**
 * ELSRPGDeathState
 *
 *	Defines current state of death.
 */
UENUM(BlueprintType)
enum class ELSRPGDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};


/**
 * ULSRPGHealthComponent
 *
 *	An actor component used to handle anything related to health.
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class LSRPG_API ULSRPGHealthComponent : public ULSRPGComponentBase
{
	GENERATED_BODY()

public:

	ULSRPGHealthComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the health component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Lyra|Health")
	static ULSRPGHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<ULSRPGHealthComponent>() : nullptr); }


	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	void InitializeWithAbilitySystem(ULSRPGAbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	void UninitializeFromAbilitySystem();


	// Returns the current health value.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	float GetHealth() const;


	// Returns the current maximum health value.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	float GetMaxHealth() const;

	// Returns the current health value.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	float GetMana() const;

	// Returns the current maximum health value.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	float GetMaxMana() const;

	// Returns the current health in the range [0.0, 1.0].
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	float GetHealthNormalized() const;

	// Returns the current health in the range [0.0, 1.0].
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	float GetManaNormalized() const;


	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	ELSRPGDeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Lyra|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > ELSRPGDeathState::NotDead); }

	// Begins the death sequence for the owner.
	virtual void StartDeath();

	// Ends the death sequence for the owner.
	virtual void FinishDeath();

	// Applies enough damage to kill the owner.
	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);


public:

	// Delegate fired when the health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FLSRPGHealth_AttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FLSRPGHealth_AttributeChanged OnMaxHealthChanged;

	// Delegate fired when the mana value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FLSRPGHealth_AttributeChanged OnManaChanged;

	// Delegate fired when the max mana value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FLSRPGHealth_AttributeChanged OnMaxManaChanged;



	// Delegate fired when the death sequence has started.
	UPROPERTY(BlueprintAssignable)
	FLSRPGHealth_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	UPROPERTY(BlueprintAssignable)
	FLSRPGHealth_DeathEvent OnDeathFinished;


protected:

	virtual void OnUnregister() override;

	void ClearGameplayTags();

	virtual void HandleHealthChanged(AActor* DamageInstigator,
	AActor* DamageCauser,
	const FGameplayEffectSpec* DamageEffectSpec,
	float DamageMagnitude, float OldValue, 
	float NewValue);

	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, 
	AActor* DamageCauser, 
	const FGameplayEffectSpec* DamageEffectSpec, 
	float DamageMagnitude, 
	float OldValue, 
	float NewValue);
	virtual void HandleOutOfHealth(AActor* DamageInstigator,
	AActor* DamageCauser, 
	const FGameplayEffectSpec* DamageEffectSpec,
	float DamageMagnitude, 
	float OldValue, 
	float NewValue);

	virtual void HandleManaChanged(AActor* DamageInstigator, 
	AActor* DamageCauser,
	const FGameplayEffectSpec* DamageEffectSpec,
	float DamageMagnitude, 
	float OldValue, 
	float NewValue);
	
	
	virtual void HandleMaxManaChanged(AActor* DamageInstigator,
	AActor* DamageCauser, 
	const FGameplayEffectSpec* DamageEffectSpec,
	float DamageMagnitude, 
	float OldValue, 
	float NewValue);



	UFUNCTION()
	virtual void OnRep_DeathState(ELSRPGDeathState OldDeathState);


protected:

	// Ability system used by this component.
	UPROPERTY()
	TObjectPtr<ULSRPGAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
	TObjectPtr<const ULSRPGCharacterAttributeSet> HealthSet;




	// Replicated state used to handle dying.
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	ELSRPGDeathState DeathState;


};
