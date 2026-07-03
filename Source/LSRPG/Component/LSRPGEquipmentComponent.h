#pragma once


#include "CoreMinimal.h"

#include "LSRPGItem.h"
#include "LSRPGComponentBase.h"

#include  "GameplayEffectTypes.h"

#include "LSRPGEquipmentComponent.generated.h"

class ALSRPGCharacterBase;
class ULSRPGAbilitySystemComponent;
class ULSRPGItem;
class ULSRPGEquipment;


USTRUCT(BlueprintType)
struct  FLSRPGEquipmentItem
{
	GENERATED_USTRUCT_BODY()

public:
	FLSRPGEquipmentItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPGInventoryItem)
	ULSRPGEquipment* RPGEquimentItemPointer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPGInventoryItem)
	FActiveGameplayEffectHandle ActiveEquipmentEffectHandle;

	bool IsValid() const;

	void ResetSelf();

};

DECLARE_DELEGATE_OneParam(FOnEquipmentItemChangedNative, const TArray<FLSRPGEquipmentItem>&);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LSRPG_API ULSRPGEquipmentComponent : public ULSRPGComponentBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULSRPGEquipmentComponent(const FObjectInitializer& ObjectInitializer);

	//装备UI绑定的代理
	FOnEquipmentItemChangedNative  OnEquipmentItemChanged;

	UFUNCTION()
	void OnRep_EquipmentItems();


	void CallServerDownLoadInfo();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//直接寻找一个空的装备进行添加,如果没有就添加失败
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool AddEquipmentItem(const FLSRPGEquipmentItem& NewEquipmentItem);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void UndockEquipmentByID(int32 InUndockEquipmentID);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool RemoveEquipmentItem(int32 Index_Remove);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void SwapEquipmentItem(int32 Index_i, int32 Index_j);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void    SwapFromInvToEqu(int32 InInventory_Index, int32 InEquipment_Index);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void    SwapFromEquToInv(int32 InEquipment_Index, int32 InInventory_Index);









	//通知UI刷新界面
	UFUNCTION(Client, Reliable)
	void EquipmentItemChanged(const TArray<FLSRPGEquipmentItem>& InEquipmentItems);



protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_EquipmentItems, Category = Equipment)
	TArray<FLSRPGEquipmentItem> EquipmentItems;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;



};