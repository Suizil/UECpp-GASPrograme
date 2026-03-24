#include "XGRPGCharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"


#include "XGRPGAbilitySystemComponent.h"
#include "XGRPGPlayerController.h"
#include "XGRPGPlayerState.h"
#include "XGRPGGameplayAbility.h"
#include "AbilitySystem/Attributes/XGRPGCharacterAttributeSet.h"
#include "XGRPGComboComponent.h"
#include "XGRPGHealthComponent.h"
#include "XGRPGInventoryComponent.h"
#include "XGRPGEquipmentComponent.h"


AXGRPGCharacterBase::AXGRPGCharacterBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;


	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UXGRPGAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);


	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	CharacterSet = CreateDefaultSubobject<UXGRPGCharacterAttributeSet>(TEXT("CharacterSet"));


	//连击组件
	ComboComponent = CreateDefaultSubobject<UXGRPGComboComponent>(TEXT("ComboComponent"));
	ComboComponent->SetIsReplicated(false);

	//生命组件
	HealthComponent = CreateDefaultSubobject<UXGRPGHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->SetIsReplicated(true);
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	//仓库组件
	InventoryComponent = CreateDefaultSubobject<UXGRPGInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);

	//装备组件
	EquipmentComponent = CreateDefaultSubobject<UXGRPGEquipmentComponent>(TEXT("EquipmentComponent"));
	EquipmentComponent->SetIsReplicated(true);


	NetUpdateFrequency = 100.0f;


}

AXGRPGPlayerController* AXGRPGCharacterBase::GetXGRPGPlayerController() const
{
	 return CastChecked<AXGRPGPlayerController>(Controller, ECastCheckedType::NullAllowed); 

}

AXGRPGPlayerState* AXGRPGCharacterBase::GetXGRPGPlayerState() const
{
	return CastChecked<AXGRPGPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UAbilitySystemComponent* AXGRPGCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AXGRPGCharacterBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UXGRPGAbilitySystemComponent* XGRPGASC = GetXGRPGAbilitySystemComponent())
	{
		XGRPGASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool AXGRPGCharacterBase::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UXGRPGAbilitySystemComponent* XGRPGASC = GetXGRPGAbilitySystemComponent())
	{
		return XGRPGASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool AXGRPGCharacterBase::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UXGRPGAbilitySystemComponent* XGRPGASC = GetXGRPGAbilitySystemComponent())
	{
		return XGRPGASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool AXGRPGCharacterBase::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UXGRPGAbilitySystemComponent* XGRPGASC = GetXGRPGAbilitySystemComponent())
	{
		return XGRPGASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void AXGRPGCharacterBase::ActiveSkillByInventoryId_Implementation(int32 InInventoryId)
{
	if (InventoryComponent)
	{

		InventoryComponent->ActiveSkillByInventoryId(InInventoryId);

	}


}

void AXGRPGCharacterBase::ClientRPCFunction_Implementation(FGameplayTagContainer OutAbilityTag, float CooldownTime)
{

	AbilityCoolDownDelegate.Broadcast(OutAbilityTag, CooldownTime);

}

void AXGRPGCharacterBase::CallServerDownLoadInfo_Implementation()
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
void AXGRPGCharacterBase::SwapInventoryItem_Implementation(int32 Index_i, int32 Index_j)
{
	if (InventoryComponent)
	{

		InventoryComponent->SwapInventoryItem(Index_i, Index_j);

	}

}
void AXGRPGCharacterBase::UndockEquipmentByID_Implementation(int32 InUndockEquipmentID)
{
	if (EquipmentComponent)
	{

		EquipmentComponent->UndockEquipmentByID(InUndockEquipmentID);

	}

}


void AXGRPGCharacterBase::SwapEquipmentItem_Implementation(int32 Index_i, int32 Index_j)
{
	if (EquipmentComponent)
	{

		EquipmentComponent->SwapEquipmentItem(Index_i, Index_j);

	}

}
void AXGRPGCharacterBase::SwapFromInvToEqu_Implementation(int32 InInventory_Index, int32 InEquipment_Index)
{
	if (EquipmentComponent)
	{

		EquipmentComponent->SwapFromInvToEqu(InInventory_Index, InEquipment_Index);

	}

}


void AXGRPGCharacterBase::SwapFromEquToInv_Implementation(int32 InEquipment_Index, int32 InInventory_Index)
{
	if (EquipmentComponent)
	{
		EquipmentComponent->SwapFromEquToInv(InEquipment_Index, InInventory_Index);
	}

}



void AXGRPGCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	//GAS只在服务器上注册
	if (AbilitySystemComponent && GetLocalRole() == ENetRole::ROLE_Authority)
	{

		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		//临时批量注册能力
		for (auto& TmpAbilityPair : AbilitiesToAdd)
		{

			UXGRPGGameplayAbility* AbilityCDO = TmpAbilityPair.Value->GetDefaultObject<UXGRPGGameplayAbility>();

			FGameplayAbilitySpec AbilitySpec(AbilityCDO, 1);

			AbilitySpec.SourceObject = this;
			AbilitySpec.DynamicAbilityTags.AddTag(TmpAbilityPair.Key);

			//向GAS系统注册技能拿到handle
			const FGameplayAbilitySpecHandle AbilitySpecHandle = AbilitySystemComponent->GiveAbility(AbilitySpec);

			AbilitiesToActive.Add(TmpAbilityPair.Key, AbilitySpecHandle);

		}

		UXGRPGGameplayAbility* DeathAbilityCDO = DeathAbilityClass->GetDefaultObject<UXGRPGGameplayAbility>();
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

void AXGRPGCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//GAS只在服务器上注册

	if (AbilitySystemComponent && GetLocalRole() == ENetRole::ROLE_Authority)
	{
		AbilitySystemComponent->ClearAllAbilities();
	}
	Super::EndPlay(EndPlayReason);
}

void AXGRPGCharacterBase::OnDeathStarted(AActor* OwningActor)
{

	DisableMovementAndCollision();
}


void AXGRPGCharacterBase::OnDeathFinished(AActor* OwningActor)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);

}

void AXGRPGCharacterBase::DisableMovementAndCollision()
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

void AXGRPGCharacterBase::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}

void AXGRPGCharacterBase::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	HealthComponent->UninitializeFromAbilitySystem();

	SetActorHiddenInGame(true);
}


