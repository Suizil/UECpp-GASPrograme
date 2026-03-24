#include "XGRPGInventoryComponent.h"

#include "Net/UnrealNetwork.h"

#include "XGRPGPotion.h"
#include "XGRPGEquipment.h"

#include "AbilitySystemComponent.h"
#include "XGRPGGameplayAbility.h"
#include "XGRPGCharacterBase.h"

#include "XGRPGEquipmentComponent.h"


FXGRPGInventoryItem::FXGRPGInventoryItem()
	: RPGItem(nullptr)
	, ItemCount(0)
{





}

bool FXGRPGInventoryItem::IsValid() const
{
	if (RPGItem && ItemCount > 0)
	{
		return true;

	}
	return false;
}

void FXGRPGInventoryItem::ResetSelf()
{
	RPGItem = nullptr;
	ItemCount = 0;


}

UXGRPGInventoryComponent::UXGRPGInventoryComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	InventoryItems.AddDefaulted(30);

	SetIsReplicatedByDefault(true);

}

void UXGRPGInventoryComponent::OnRep_InventoryItems()
{
	OnInventoryItemChanged.ExecuteIfBound(InventoryItems);
}

void UXGRPGInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UXGRPGInventoryComponent::ActiveSkillByInventoryId(int32 InInventoryId)
{
	if (InventoryItems[InInventoryId].IsValid())
	{
		if (UXGRPGPotion* InPotion = Cast<UXGRPGPotion>(InventoryItems[InInventoryId].RPGItem))
		{
			UAbilitySystemComponent* AbilitySystemComponent = Cast<AXGRPGCharacterBase>(GetOwner())->GetAbilitySystemComponent();

			UXGRPGGameplayAbility* InGA = Cast<UXGRPGGameplayAbility>(InPotion->GrantedAbility->GetDefaultObject());

			FGameplayAbilitySpecHandle  Handle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(InGA));

			AbilitySystemComponent->TryActivateAbility(Handle);

			//自动移除handle
			AbilitySystemComponent->SetRemoveAbilityOnEnd(Handle);

		}
		else if (UXGRPGEquipment* InEquipment = Cast<UXGRPGEquipment>(InventoryItems[InInventoryId].RPGItem))
		{
			UXGRPGEquipmentComponent* EquipmentComponent = 
			Cast<UXGRPGEquipmentComponent>(GetOwner()->FindComponentByClass(UXGRPGEquipmentComponent::StaticClass()));


			//通知服务器装备装备
			FXGRPGEquipmentItem InEquipmentItem;
			InEquipmentItem.RPGEquimentItemPointer = InEquipment;

			//通知装备组件装备一个装备
			if (EquipmentComponent)
			{
				bool bAddEquipment = EquipmentComponent->AddEquipmentItem(InEquipmentItem);
				if (bAddEquipment)
				{
					RemoveInventoryItem(InInventoryId);

				}

			}


		}
	}
}

void UXGRPGInventoryComponent::CallServerDownLoadInfo()
{
	InventoryItemChanged(InventoryItems);
}

void UXGRPGInventoryComponent::InventoryItemChanged_Implementation(const TArray<FXGRPGInventoryItem>& InInventoryItems)
{
	OnInventoryItemChanged.ExecuteIfBound(InventoryItems);
}

bool UXGRPGInventoryComponent::AddInventoryItem(FXGRPGInventoryItem NewItem)
{
	//只在服务器操作数据
	if (HasAuthority())
	{

		if (!NewItem.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("AddInventoryItem: Failed trying to add null item!"));
			return false;

		}

		//找一找 原先的仓库有没有
		bool bFind = false;
		for (size_t i = 0; i < InventoryItems.Num(); i++)
		{
			if (InventoryItems[i].RPGItem == NewItem.RPGItem)
			{
				if (NewItem.RPGItem->MaxCount > 1)
				{

					InventoryItems[i].ItemCount += NewItem.ItemCount;
					bFind = true;
					return true;
				}
			}

		}

		//去找一下有米有空的位置
		for (size_t i = 0; i < InventoryItems.Num(); i++)
		{
			if (!InventoryItems[i].IsValid())
			{
				InventoryItems[i] = NewItem;
				return true;
			}
		}


	}
	//	仓库满了
	return false;
}

bool UXGRPGInventoryComponent::HasThisInventoryItem(FXGRPGInventoryItem NewItem)
{
	for (size_t i = 0; i < InventoryItems.Num(); i++)
	{
		if (InventoryItems[i].RPGItem == NewItem.RPGItem
		&& InventoryItems[i].ItemCount == NewItem.ItemCount)
		{
			return true;
		}
	}



	return false;
}

bool UXGRPGInventoryComponent::ReplaceInventoryItem(FXGRPGInventoryItem NewItem, int32 InReplace_ID)
{
	if (HasAuthority())
	{
		InventoryItems[InReplace_ID] = NewItem;

		return true;
	}

	return false;
}

void UXGRPGInventoryComponent::RemoveInventoryItem(int32 Index_Remove)
{
	if (HasAuthority())
	{

		InventoryItems[Index_Remove].ResetSelf();


	}
}

void UXGRPGInventoryComponent::RemoveInventoryItemNyItem(FXGRPGInventoryItem NewItem)
{

	if (HasAuthority())
	{
		for (size_t i = 0; i < InventoryItems.Num(); i++)
		{
			if (InventoryItems[i].RPGItem == NewItem.RPGItem)
			{
				//减去目标数量
				InventoryItems[i].ItemCount -= NewItem.ItemCount;

				if (InventoryItems[i].ItemCount < 0)
				{
					InventoryItems[i].ResetSelf();
				}
				break;
			}

		}
	}
}

void UXGRPGInventoryComponent::SwapInventoryItem(int32 Index_i, int32 Index_j)
{
	if (HasAuthority())
	{
		InventoryItems.Swap(Index_i, Index_j);
	}
}

UXGRPGItem* UXGRPGInventoryComponent::GetInventoryItemByID(int32 i)
{

	if (!InventoryItems[i].IsValid())
	{

		return nullptr;
	}



	return InventoryItems[i].RPGItem;

}

FXGRPGInventoryItem UXGRPGInventoryComponent::GetInventoryItemInfoByID(int32 i)
{

	return InventoryItems[i];

}

void UXGRPGInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UXGRPGInventoryComponent, InventoryItems);
}
