#include "LSRPGHealthComponent.h"

#include "Net/UnrealNetwork.h"


#include "LSRPGLogChannels.h"
#include "LSRPGGameplayTag.h"


#include "LSRPGAbilitySystemComponent.h"
#include "LSRPGCharacterAttributeSet.h"
#include "LSRPGGameplayAbility.h"

#include "LSRPGAssetManager.h"
#include "LSRPGGameData.h"


UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Damage, "Gameplay.Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageImmunity, "Gameplay.DamageImmunity");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageSelfDestruct, "Gameplay.Damage.SelfDestruct");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_FellOutOfWorld, "Gameplay.Damage.FellOutOfWorld");
UE_DEFINE_GAMEPLAY_TAG(TAG_Lyra_Damage_Message, "Lyra.Damage.Message");


ULSRPGHealthComponent::ULSRPGHealthComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
	DeathState = ELSRPGDeathState::NotDead;

}
void ULSRPGHealthComponent::InitializeWithAbilitySystem(ULSRPGAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();

	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogLSRPG, Error, TEXT("LSRPGHealthComponent: Health component for owner [%s] has already been initialized with an ability system."),
		*GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;

	if (!AbilitySystemComponent)
	{
		UE_LOG(LogLSRPG, Error, TEXT("LSRPGHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."),
		*GetNameSafe(Owner));
		return;
	}

	HealthSet = AbilitySystemComponent->GetSet<ULSRPGCharacterAttributeSet>();

	if (!HealthSet)
	{
		UE_LOG(LogLSRPG, Error, TEXT("LyraHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."),
		*GetNameSafe(Owner));
		return;
	}



	// Register to listen for attribute changes.
	HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChanged);
	HealthSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);

	HealthSet->OnManaChanged.AddUObject(this, &ThisClass::HandleManaChanged);
	HealthSet->OnMaxManaChanged.AddUObject(this, &ThisClass::HandleMaxManaChanged);

	// TEMP: Reset attributes to default values.  Eventually this will be driven by a spread sheet.
	AbilitySystemComponent->SetNumericAttributeBase(ULSRPGCharacterAttributeSet::GetHealthAttribute(), HealthSet->GetMaxHealth());
	
	ClearGameplayTags();

	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);

	OnManaChanged.Broadcast(this, HealthSet->GetMana(), HealthSet->GetMana(), nullptr);
	OnMaxManaChanged.Broadcast(this, HealthSet->GetMaxMana(), HealthSet->GetMaxMana(), nullptr);




}
void ULSRPGHealthComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();


	if (HealthSet)
	{
		HealthSet->OnHealthChanged.RemoveAll(this);
		HealthSet->OnMaxHealthChanged.RemoveAll(this);
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	HealthSet = nullptr;
	AbilitySystemComponent = nullptr;


}
float ULSRPGHealthComponent::GetHealth() const
{
	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
}
float ULSRPGHealthComponent::GetMaxHealth() const
{
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
}
float ULSRPGHealthComponent::GetMana() const
{
	return (HealthSet ? HealthSet->GetMana() : 0.0f);
}
float ULSRPGHealthComponent::GetMaxMana() const
{
	return (HealthSet ? HealthSet->GetMaxMana() : 0.0f);
}
float ULSRPGHealthComponent::GetHealthNormalized() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();

		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}

	return 0.0f;
}
float ULSRPGHealthComponent::GetManaNormalized() const
{
	if (HealthSet)
	{
		const float Mana = HealthSet->GetMana();
		const float MaxMana = HealthSet->GetMaxMana();

		return ((MaxMana > 0.0f) ? (Mana / MaxMana) : 0.0f);
	}

	return 0.0f;
}
void ULSRPGHealthComponent::StartDeath()
{
	if (DeathState != ELSRPGDeathState::NotDead)
	{
		return;
	}

	DeathState = ELSRPGDeathState::DeathStarted;



	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(LSRPGGameplayTags::Status_Death_Dying, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner);

	Owner->ForceNetUpdate();



}
void ULSRPGHealthComponent::FinishDeath()
{
	if (DeathState != ELSRPGDeathState::DeathStarted)
	{
		return;
	}

	DeathState = ELSRPGDeathState::DeathFinished;


	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(LSRPGGameplayTags::Status_Death_Dead, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner);

	Owner->ForceNetUpdate();


}
void ULSRPGHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)
{
	if ((DeathState == ELSRPGDeathState::NotDead) && AbilitySystemComponent)
	{
		const TSubclassOf<UGameplayEffect> DamageGE = ULSRPGAssetManager::GetSubclass(ULSRPGGameData::Get().DamageGameplayEffect_SetByCaller);
		if (!DamageGE)
		{
			UE_LOG(LogLSRPG, Error, TEXT("LyraHealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to find gameplay effect [%s]."),
				*GetNameSafe(GetOwner()), *ULSRPGGameData::Get().DamageGameplayEffect_SetByCaller.GetAssetName());
			return;
		}

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DamageGE, 1.0f, AbilitySystemComponent->MakeEffectContext());
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

		if (!Spec)
		{
			UE_LOG(LogLSRPG, Error, TEXT("LyraHealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to make outgoing spec for [%s]."), *GetNameSafe(GetOwner()), *GetNameSafe(DamageGE));
			return;
		}

		Spec->AddDynamicAssetTag(TAG_Gameplay_DamageSelfDestruct);

		if (bFellOutOfWorld)
		{
			Spec->AddDynamicAssetTag(TAG_Gameplay_FellOutOfWorld);
		}

		const float DamageAmount = GetMaxHealth();

		Spec->SetSetByCallerMagnitude(LSRPGGameplayTags::SetByCaller_Damage, DamageAmount);
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
	}






}
void ULSRPGHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULSRPGHealthComponent, DeathState);
}

void ULSRPGHealthComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();

	Super::OnUnregister();
}

void ULSRPGHealthComponent::ClearGameplayTags()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(LSRPGGameplayTags::Status_Death_Dying, 0);
		AbilitySystemComponent->SetLooseGameplayTagCount(LSRPGGameplayTags::Status_Death_Dead, 0);
	}
}

void ULSRPGHealthComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{

	OnHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void ULSRPGHealthComponent::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnMaxHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);

}

void ULSRPGHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{

#if WITH_SERVER_CODE

	if (AbilitySystemComponent && DamageEffectSpec)
	{

		// Send the "GameplayEvent.Death" gameplay event through the owner's ability system.  This can be used to trigger a death gameplay ability.
		{
			FGameplayEventData Payload;

			Payload.EventTag = LSRPGGameplayTags::GameplayEvent_Death;
			Payload.Instigator = DamageInstigator;
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec->Def;
			Payload.ContextHandle = DamageEffectSpec->GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;

			FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);



		}
	}


#endif // #if WITH_SERVER_CODE

}

void ULSRPGHealthComponent::HandleManaChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{

	OnManaChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void ULSRPGHealthComponent::HandleMaxManaChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnMaxManaChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void ULSRPGHealthComponent::OnRep_DeathState(ELSRPGDeathState OldDeathState)
{

	const ELSRPGDeathState NewDeathState = DeathState;

	// Revert the death state for now since we rely on StartDeath and FinishDeath to change it.
	DeathState = OldDeathState;


	if (OldDeathState > NewDeathState)
	{
		// The server is trying to set us back but we've already predicted past the server state.
		UE_LOG(LogLSRPG, Warning, TEXT("LSRPGHealthComponent: Predicted past server death state [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		return;
	}

	if (OldDeathState == ELSRPGDeathState::NotDead)
	{
		if (NewDeathState == ELSRPGDeathState::DeathStarted)
		{
			StartDeath();
		}
		else if (NewDeathState == ELSRPGDeathState::DeathFinished)
		{
			StartDeath();
			FinishDeath();
		}
		else
		{
			UE_LOG(LogLSRPG, Error, TEXT("LSRPGHealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}



	}
	else if (OldDeathState == ELSRPGDeathState::DeathStarted)
	{

		if (NewDeathState == ELSRPGDeathState::DeathFinished)
		{
			FinishDeath();
		}
		else
		{
			UE_LOG(LogLSRPG, Error, TEXT("LSRPGHealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));

		}
	}

	ensureMsgf((DeathState == NewDeathState), TEXT("LSRPGHealthComponent: Death transition failed [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));

}
