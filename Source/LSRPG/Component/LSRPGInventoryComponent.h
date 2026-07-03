#pragma once

#include "CoreMinimal.h"
#include "LSRPGItem.h"

#include "Components/GameFrameworkComponent.h"
#include "LSRPGComponentBase.h"

#include "LSRPGInventoryComponent.generated.h"



class ALSRPGCharacterBase;
class ULSRPGAbilitySystemComponent;
class ULSRPGItem;



USTRUCT(BlueprintType)
struct  FLSRPGInventoryItem
{
	GENERATED_USTRUCT_BODY()

public:
	FLSRPGInventoryItem();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPGInventoryItem)
	ULSRPGItem* RPGItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPGInventoryItem)
	int32 ItemCount;

	bool IsValid() const;

	void ResetSelf();

};

DECLARE_DELEGATE_OneParam(FOnInventoryItemChangedNative, const TArray<FLSRPGInventoryItem>&);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LSRPG_API ULSRPGInventoryComponent : public ULSRPGComponentBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULSRPGInventoryComponent(const FObjectInitializer& ObjectInitializer);

	//给UI绑定的代理
	//仓库的代理
	FOnInventoryItemChangedNative OnInventoryItemChanged;

	UFUNCTION()
	void OnRep_InventoryItems();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

public:

	void ActiveSkillByInventoryId(int32  InInventoryId);

	void CallServerDownLoadInfo();

	//仅供内部使用
	//通知UI刷新界面
	//为什么会有参数 因为是RPC需要同步,如果在方法体内执行内,拖取的数据是客户端的
	UFUNCTION(Client, Reliable)
	void InventoryItemChanged(const TArray<FLSRPGInventoryItem>& InInventoryItems);


	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool AddInventoryItem(FLSRPGInventoryItem NewItem);

	UFUNCTION(BlueprintPure, Category = Inventory)
	bool HasThisInventoryItem(FLSRPGInventoryItem NewItem);

	//添加到制定位置
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool ReplaceInventoryItem(FLSRPGInventoryItem NewItem, int32 InReplace_ID);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void RemoveInventoryItem(int32 Index_Remove);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void RemoveInventoryItemNyItem(FLSRPGInventoryItem NewItem);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void SwapInventoryItem(int32 Index_i, int32 Index_j);

	//获取制定仓库指定栏位的物品
	UFUNCTION(BlueprintCallable, Category = Inventory)
	ULSRPGItem* GetInventoryItemByID(int32 i);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	FLSRPGInventoryItem GetInventoryItemInfoByID(int32 i);





protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_InventoryItems, Category = Inventory)
	TArray<FLSRPGInventoryItem> InventoryItems;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;



};

