#include "XGRPGHealthComponent.h"

#include "Net/UnrealNetwork.h"


#include "XGRPGLogChannels.h"
#include "XGRPGGameplayTag.h"


#include "XGRPGAbilitySystemComponent.h"
#include "XGRPGCharacterAttributeSet.h"
#include "XGRPGGameplayAbility.h"

#include "XGRPGAssetManager.h"
#include "XGRPGGameData.h"


UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Damage, "Gameplay.Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageImmunity, "Gameplay.DamageImmunity");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageSelfDestruct, "Gameplay.Damage.SelfDestruct");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_FellOutOfWorld, "Gameplay.Damage.FellOutOfWorld");
UE_DEFINE_GAMEPLAY_TAG(TAG_Lyra_Damage_Message, "Lyra.Damage.Message");


UXGRPGHealthComponent::UXGRPGHealthComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
	DeathState = EXGRPGDeathState::NotDead;

}
void UXGRPGHealthComponent::InitializeWithAbilitySystem(UXGRPGAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();

	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogXGRPG, Error, TEXT("XGRPGHealthComponent: Health component for owner [%s] has already been initialized with an ability system."), 
		*GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;

	if (!AbilitySystemComponent)
	{
		UE_LOG(LogXGRPG, Error, TEXT("XGRPGHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."), 
		*GetNameSafe(Owner));
		return;
	}

	HealthSet = AbilitySystemComponent->GetSet<UXGRPGCharacterAttributeSet>();

	if (!HealthSet)
	{
		UE_LOG(LogXGRPG, Error, TEXT("LyraHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."), 
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
	AbilitySystemComponent->SetNumericAttributeBase(UXGRPGCharacterAttributeSet::GetHealthAttribute(), HealthSet->GetMaxHealth());
	
	ClearGameplayTags();

	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);

	OnManaChanged.Broadcast(this, HealthSet->GetMana(), HealthSet->GetMana(), nullptr);
	OnMaxManaChanged.Broadcast(this, HealthSet->GetMaxMana(), HealthSet->GetMaxMana(), nullptr);




}
void UXGRPGHealthComponent::UninitializeFromAbilitySystem()
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
float UXGRPGHealthComponent::GetHealth() const
{
	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
}
float UXGRPGHealthComponent::GetMaxHealth() const
{
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
}
float UXGRPGHealthComponent::GetMana() const
{
	return (HealthSet ? HealthSet->GetMana() : 0.0f);
}
float UXGRPGHealthComponent::GetMaxMana() const
{
	return (HealthSet ? HealthSet->GetMaxMana() : 0.0f);
}
float UXGRPGHealthComponent::GetHealthNormalized() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();

		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}

	return 0.0f;
}
float UXGRPGHealthComponent::GetManaNormalized() const
{
	if (HealthSet)
	{
		const float Mana = HealthSet->GetMana();
		const float MaxMana = HealthSet->GetMaxMana();

		return ((MaxMana > 0.0f) ? (Mana / MaxMana) : 0.0f);
	}

	return 0.0f;
}
void UXGRPGHealthComponent::StartDeath()
{
	if (DeathState != EXGRPGDeathState::NotDead)
	{
		return;
	}

	DeathState = EXGRPGDeathState::DeathStarted;



	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(XGRPGGameplayTags::Status_Death_Dying, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner);

	Owner->ForceNetUpdate();



}
void UXGRPGHealthComponent::FinishDeath()
{
	if (DeathState != EXGRPGDeathState::DeathStarted)
	{
		return;
	}

	DeathState = EXGRPGDeathState::DeathFinished;


	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(XGRPGGameplayTags::Status_Death_Dead, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner);

	Owner->ForceNetUpdate();


}
void UXGRPGHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)
{
	if ((DeathState == EXGRPGDeathState::NotDead) && AbilitySystemComponent)
	{
		const TSubclassOf<UGameplayEffect> DamageGE = UXGRPGAssetManager::GetSubclass(UXGRPGGameData::Get().DamageGameplayEffect_SetByCaller);
		if (!DamageGE)
		{
			UE_LOG(LogXGRPG, Error, TEXT("LyraHealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to find gameplay effect [%s]."),
				*GetNameSafe(GetOwner()), *UXGRPGGameData::Get().DamageGameplayEffect_SetByCaller.GetAssetName());
			return;
		}

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DamageGE, 1.0f, AbilitySystemComponent->MakeEffectContext());
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

		if (!Spec)
		{
			UE_LOG(LogXGRPG, Error, TEXT("LyraHealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to make outgoing spec for [%s]."), *GetNameSafe(GetOwner()), *GetNameSafe(DamageGE));
			return;
		}

		Spec->AddDynamicAssetTag(TAG_Gameplay_DamageSelfDestruct);

		if (bFellOutOfWorld)
		{
			Spec->AddDynamicAssetTag(TAG_Gameplay_FellOutOfWorld);
		}

		const float DamageAmount = GetMaxHealth();

		Spec->SetSetByCallerMagnitude(XGRPGGameplayTags::SetByCaller_Damage, DamageAmount);
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
	}






}
void UXGRPGHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UXGRPGHealthComponent, DeathState);
}

void UXGRPGHealthComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();

	Super::OnUnregister();
}

void UXGRPGHealthComponent::ClearGameplayTags()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(XGRPGGameplayTags::Status_Death_Dying, 0);
		AbilitySystemComponent->SetLooseGameplayTagCount(XGRPGGameplayTags::Status_Death_Dead, 0);
	}
}

void UXGRPGHealthComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{

	OnHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UXGRPGHealthComponent::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnMaxHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);

}

void UXGRPGHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{

#if WITH_SERVER_CODE

	if (AbilitySystemComponent && DamageEffectSpec)
	{

		// Send the "GameplayEvent.Death" gameplay event through the owner's ability system.  This can be used to trigger a death gameplay ability.
		{
			FGameplayEventData Payload;

			Payload.EventTag = XGRPGGameplayTags::GameplayEvent_Death;
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

void UXGRPGHealthComponent::HandleManaChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{

	OnManaChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UXGRPGHealthComponent::HandleMaxManaChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnMaxManaChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UXGRPGHealthComponent::OnRep_DeathState(EXGRPGDeathState OldDeathState)
{

	const EXGRPGDeathState NewDeathState = DeathState;

	// Revert the death state for now since we rely on StartDeath and FinishDeath to change it.
	DeathState = OldDeathState;


	if (OldDeathState > NewDeathState)
	{
		// The server is trying to set us back but we've already predicted past the server state.
		UE_LOG(LogXGRPG, Warning, TEXT("XGRPGHealthComponent: Predicted past server death state [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		return;
	}

	if (OldDeathState == EXGRPGDeathState::NotDead)
	{
		if (NewDeathState == EXGRPGDeathState::DeathStarted)
		{
			StartDeath();
		}
		else if (NewDeathState == EXGRPGDeathState::DeathFinished)
		{
			StartDeath();
			FinishDeath();
		}
		else
		{
			UE_LOG(LogXGRPG, Error, TEXT("XGRPGHealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}



	}
	else if (OldDeathState == EXGRPGDeathState::DeathStarted)
	{

		if (NewDeathState == EXGRPGDeathState::DeathFinished)
		{
			FinishDeath();
		}
		else
		{
			UE_LOG(LogXGRPG, Error, TEXT("XGRPGHealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));

		}
	}

	ensureMsgf((DeathState == NewDeathState), TEXT("XGRPGHealthComponent: Death transition failed [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));

}
