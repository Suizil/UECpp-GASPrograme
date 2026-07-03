#include "LSRPGCharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"


#include "LSRPGAbilitySystemComponent.h"
#include "LSRPGPlayerController.h"
#include "LSRPGPlayerState.h"
#include "LSRPGGameplayAbility.h"
#include "AbilitySystem/Attributes/LSRPGCharacterAttributeSet.h"
#include "LSRPGComboComponent.h"
#include "LSRPGHealthComponent.h"
#include "LSRPGInventoryComponent.h"
#include "LSRPGEquipmentComponent.h"


ALSRPGCharacterBase::ALSRPGCharacterBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;


	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<ULSRPGAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);


	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	CharacterSet = CreateDefaultSubobject<ULSRPGCharacterAttributeSet>(TEXT("CharacterSet"));


	//连击组件
	ComboComponent = CreateDefaultSubobject<ULSRPGComboComponent>(TEXT("ComboComponent"));
	ComboComponent->SetIsReplicated(false);

	//生命组件
	HealthComponent = CreateDefaultSubobject<ULSRPGHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->SetIsReplicated(true);
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	//仓库组件
	InventoryComponent = CreateDefaultSubobject<ULSRPGInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);

	//装备组件
	EquipmentComponent = CreateDefaultSubobject<ULSRPGEquipmentComponent>(TEXT("EquipmentComponent"));
	EquipmentComponent->SetIsReplicated(true);


	NetUpdateFrequency = 100.0f;


}

ALSRPGPlayerController* ALSRPGCharacterBase::GetLSRPGPlayerController() const
{
	 return CastChecked<ALSRPGPlayerController>(Controller, ECastCheckedType::NullAllowed);

}

ALSRPGPlayerState* ALSRPGCharacterBase::GetLSRPGPlayerState() const
{
	return CastChecked<ALSRPGPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UAbilitySystemComponent* ALSRPGCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ALSRPGCharacterBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const ULSRPGAbilitySystemComponent* LSRPGASC = GetLSRPGAbilitySystemComponent())
	{
		LSRPGASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ALSRPGCharacterBase::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const ULSRPGAbilitySystemComponent* LSRPGASC = GetLSRPGAbilitySystemComponent())
	{
		return LSRPGASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ALSRPGCharacterBase::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const ULSRPGAbilitySystemComponent* LSRPGASC = GetLSRPGAbilitySystemComponent())
	{
		return LSRPGASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ALSRPGCharacterBase::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const ULSRPGAbilitySystemComponent* LSRPGASC = GetLSRPGAbilitySystemComponent())
	{
		return LSRPGASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void ALSRPGCharacterBase::ActiveSkillByInventoryId_Implementation(int32 InInventoryId)
{
	if (InventoryComponent)
	{

		InventoryComponent->ActiveSkillByInventoryId(InInventoryId);

	}


}

void ALSRPGCharacterBase::ClientRPCFunction_Implementation(FGameplayTagContainer OutAbilityTag, float CooldownTime)
{

	AbilityCoolDownDelegate.Broadcast(OutAbilityTag, CooldownTime);

}

void ALSRPGCharacterBase::CallServerDownLoadInfo_Implementation()
{
	if (InventoryComponent)
	{
		InventoryComponent->CallServerDownLoadInfo();
	}

	if (EquipmentComponent)
	{
		EquipmentComponent->CallServerDownLoadInfo();
	}


}
void ALSRPGCharacterBase::SwapInventoryItem_Implementation(int32 Index_i, int32 Index_j)
{
	if (InventoryComponent)
	{

		InventoryComponent->SwapInventoryItem(Index_i, Index_j);

	}

}
void ALSRPGCharacterBase::UndockEquipmentByID_Implementation(int32 InUndockEquipmentID)
{
	if (EquipmentComponent)
	{

		EquipmentComponent->UndockEquipmentByID(InUndockEquipmentID);

	}

}


void ALSRPGCharacterBase::SwapEquipmentItem_Implementation(int32 Index_i, int32 Index_j)
{
	if (EquipmentComponent)
	{

		EquipmentComponent->SwapEquipmentItem(Index_i, Index_j);

	}

}
void ALSRPGCharacterBase::SwapFromInvToEqu_Implementation(int32 InInventory_Index, int32 InEquipment_Index)
{
	if (EquipmentComponent)
	{

		EquipmentComponent->SwapFromInvToEqu(InInventory_Index, InEquipment_Index);

	}

}


void ALSRPGCharacterBase::SwapFromEquToInv_Implementation(int32 InEquipment_Index, int32 InInventory_Index)
{
	if (EquipmentComponent)
	{
		EquipmentComponent->SwapFromEquToInv(InEquipment_Index, InInventory_Index);
	}

}



void ALSRPGCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	//GAS只在服务器上注册
	if (AbilitySystemComponent && GetLocalRole() == ENetRole::ROLE_Authority)
	{

		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		//临时批量注册能力
		for (auto& TmpAbilityPair : AbilitiesToAdd)
		{

			ULSRPGGameplayAbility* AbilityCDO = TmpAbilityPair.Value->GetDefaultObject<ULSRPGGameplayAbility>();

			FGameplayAbilitySpec AbilitySpec(AbilityCDO, 1);

			AbilitySpec.SourceObject = this;
			AbilitySpec.DynamicAbilityTags.AddTag(TmpAbilityPair.Key);

			//向GAS系统注册技能拿到handle
			const FGameplayAbilitySpecHandle AbilitySpecHandle = AbilitySystemComponent->GiveAbility(AbilitySpec);

			AbilitiesToActive.Add(TmpAbilityPair.Key, AbilitySpecHandle);

		}

		ULSRPGGameplayAbility* DeathAbilityCDO = DeathAbilityClass->GetDefaultObject<ULSRPGGameplayAbility>();
		FGameplayAbilitySpec DeathAbilityCDOSpec(DeathAbilityCDO, 1);

		DeathAbilityCDOSpec.SourceObject = this;

		DeathAbilityHandle = AbilitySystemComponent->GiveAbility(DeathAbilityCDOSpec);

		AbilitySystemComponent->AbilityCommittedCallbacks.AddLambda([this](UGameplayAbility* InGameplayAbility) {

			float CoolDownTime = InGameplayAbility->GetCooldownTimeRemaining();

			FGameplayTagContainer  AbilityTags = InGameplayAbility->AbilityTags;

			ClientRPCFunction(AbilityTags, CoolDownTime);

			});



	}
	HealthComponent->InitializeWithAbilitySystem(AbilitySystemComponent);
}

void ALSRPGCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//GAS只在服务器上注册

	if (AbilitySystemComponent && GetLocalRole() == ENetRole::ROLE_Authority)
	{
		AbilitySystemComponent->ClearAllAbilities();
	}
	Super::EndPlay(EndPlayReason);
}

void ALSRPGCharacterBase::OnDeathStarted(AActor* OwningActor)
{

	DisableMovementAndCollision();
}


void ALSRPGCharacterBase::OnDeathFinished(AActor* OwningActor)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);

}

void ALSRPGCharacterBase::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UCharacterMovementComponent* MoveComp = CastChecked<UCharacterMovementComponent>(GetCharacterMovement());
	MoveComp->StopMovementImmediately();
	MoveComp->DisableMovement();
}

void ALSRPGCharacterBase::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}

void ALSRPGCharacterBase::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	HealthComponent->UninitializeFromAbilitySystem();

	SetActorHiddenInGame(true);
}


