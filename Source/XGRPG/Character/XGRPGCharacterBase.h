#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"

#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"


#include "GameplayAbilitySpecHandle.h"

#include "XGRPGCharacterBase.generated.h"


class UXGRPGAbilitySystemComponent;
class AXGRPGPlayerController;
class AXGRPGPlayerState;
class UXGRPGGameplayAbility;

class UXGRPGComboComponent;
class UXGRPGHealthComponent;

class UXGRPGInventoryComponent;
class UXGRPGEquipmentComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGenericAbilityCoolDownDelegate, FGameplayTagContainer, OutAbilityTag, float, CooldownTime);


/**
 * AXGRPGCharacterBase
 *
 *	The base character pawn class used by this project.
 *	Responsible for sending events to pawn components.
 *	New behavior should be added via pawn components when possible.
 *
 */
UCLASS(config = Game)
class XGRPG_API AXGRPGCharacterBase : public ACharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:

	AXGRPGCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:

	UFUNCTION(BlueprintCallable, Category = "XGRPG|Character")
	AXGRPGPlayerController* GetXGRPGPlayerController() const ;

	UFUNCTION(BlueprintCallable, Category = "XGRPG|Character")
	AXGRPGPlayerState* GetXGRPGPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "XGRPG|Character")
	FORCEINLINE UXGRPGAbilitySystemComponent* GetXGRPGAbilitySystemComponent() const { return AbilitySystemComponent; };

	UFUNCTION(BlueprintCallable, Category = "XGRPG|Character")
	FORCEINLINE UXGRPGComboComponent* GetXGRPGComboComponent() const { return ComboComponent; };

	UFUNCTION(BlueprintCallable, Category = "XGRPG|Character")
	FORCEINLINE UXGRPGHealthComponent* GetXGRPGHealthComponent() const { return HealthComponent; };

	UFUNCTION(BlueprintCallable, Category = "XGRPG|Character")
	FORCEINLINE UXGRPGInventoryComponent* GetXGRPGInventoryComponent() const { return InventoryComponent; };

	UFUNCTION(BlueprintCallable, Category = "XGRPG|Character")
	FORCEINLINE UXGRPGEquipmentComponent* GetXGRPGEquipmentComponent() const { return EquipmentComponent; };

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const ;



	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
protected:


	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientRPCFunction(FGameplayTagContainer OutAbilityTag, float CooldownTime);




public:

	UFUNCTION(Server, Reliable)
	void ActiveSkillByInventoryId(int32  InInventoryId);

	UFUNCTION(Server, Reliable)
	void CallServerDownLoadInfo();

	UFUNCTION(Server, Reliable)
	void SwapInventoryItem(int32 Index_i, int32 Index_j);

	UFUNCTION(Server, Reliable)
	void UndockEquipmentByID(int32 InUndockEquipmentID);

	UFUNCTION(Server, Reliable)
	void SwapEquipmentItem(int32 Index_i, int32 Index_j);

	UFUNCTION(Server, Reliable)
	void SwapFromInvToEqu(int32 InInventory_Index, int32 InEquipment_Index);


	UFUNCTION(Server, Reliable)
	void SwapFromEquToInv(int32 InEquipment_Index, int32 InInventory_Index);


	UPROPERTY(BlueprintAssignable)
	FGenericAbilityCoolDownDelegate AbilityCoolDownDelegate;




protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	void DisableMovementAndCollision();

	void DestroyDueToDeath();
	void UninitAndDestroy();

	// Called when the death sequence for the character has completed
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnDeathFinished"))
	void K2_OnDeathFinished();




protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "XGRPG|Ability", meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag, TSubclassOf<UXGRPGGameplayAbility>> AbilitiesToAdd;

	TMap<FGameplayTag, FGameplayAbilitySpecHandle> AbilitiesToActive;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "XGRPG|Ability", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UXGRPGGameplayAbility> DeathAbilityClass;

	FGameplayAbilitySpecHandle DeathAbilityHandle;


protected:


	//引入我们的GAS组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XGRPG|Character", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UXGRPGAbilitySystemComponent> AbilitySystemComponent;

	// Health attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class UXGRPGCharacterAttributeSet> CharacterSet;


	//连击组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XGRPG|Character", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UXGRPGComboComponent> ComboComponent;

	//生命组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XGRPG|Character", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UXGRPGHealthComponent> HealthComponent;

	//仓库组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XGRPG|Character", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UXGRPGInventoryComponent> InventoryComponent;

	//装备组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XGRPG|Character", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UXGRPGEquipmentComponent> EquipmentComponent;




};