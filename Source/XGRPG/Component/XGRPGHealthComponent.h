#pragma once

#include "CoreMinimal.h"

#include "XGRPGComponentBase.h"

#include "NativeGameplayTags.h"

#include "XGRPGHealthComponent.generated.h"


class UXGRPGHealthComponent;

class UXGRPGAbilitySystemComponent;
class UXGRPGCharacterAttributeSet;

class UObject;
struct FFrame;
struct FGameplayEffectSpec;




DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FXGRPGHealth_DeathEvent, AActor*, OwningActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FXGRPGHealth_AttributeChanged, 
UXGRPGHealthComponent*,HealthComponent, 
float, OldValue,
float, NewValue, 
AActor*, Instigator);


UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_FellOutOfWorld);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Lyra_Damage_Message);



/**
 * EXGRPGDeathState
 *
 *	Defines current state of death.
 */
UENUM(BlueprintType)
enum class EXGRPGDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};


/**
 * UXGRPGHealthComponent
 *
 *	An actor component used to handle anything related to health.
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class XGRPG_API UXGRPGHealthComponent : public UXGRPGComponentBase
{
	GENERATED_BODY()

public:

	UXGRPGHealthComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the health component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Lyra|Health")
	static UXGRPGHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UXGRPGHealthComponent>() : nullptr); }


	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	void InitializeWithAbilitySystem(UXGRPGAbilitySystemComponent* InASC);

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
	EXGRPGDeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Lyra|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > EXGRPGDeathState::NotDead); }

	// Begins the death sequence for the owner.
	virtual void StartDeath();

	// Ends the death sequence for the owner.
	virtual void FinishDeath();

	// Applies enough damage to kill the owner.
	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);


public:

	// Delegate fired when the health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FXGRPGHealth_AttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FXGRPGHealth_AttributeChanged OnMaxHealthChanged;

	// Delegate fired when the mana value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FXGRPGHealth_AttributeChanged OnManaChanged;

	// Delegate fired when the max mana value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FXGRPGHealth_AttributeChanged OnMaxManaChanged;



	// Delegate fired when the death sequence has started.
	UPROPERTY(BlueprintAssignable)
	FXGRPGHealth_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	UPROPERTY(BlueprintAssignable)
	FXGRPGHealth_DeathEvent OnDeathFinished;


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
	virtual void OnRep_DeathState(EXGRPGDeathState OldDeathState);


protected:

	// Ability system used by this component.
	UPROPERTY()
	TObjectPtr<UXGRPGAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
	TObjectPtr<const UXGRPGCharacterAttributeSet> HealthSet;




	// Replicated state used to handle dying.
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EXGRPGDeathState DeathState;


};