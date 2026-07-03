#include "LSRPGInventoryComponent.h"

#include "Net/UnrealNetwork.h"

#include "LSRPGPotion.h"
#include "LSRPGEquipment.h"

#include "AbilitySystemComponent.h"
#include "LSRPGGameplayAbility.h"
#include "LSRPGCharacterBase.h"

#include "LSRPGEquipmentComponent.h"


FLSRPGInventoryItem::FLSRPGInventoryItem()
	: RPGItem(nullptr)
	, ItemCount(0)
{





}

bool FLSRPGInventoryItem::IsValid() const
{
	if (RPGItem && ItemCount > 0)
	{
		return true;

	}
	return false;
}

void FLSRPGInventoryItem::ResetSelf()
{
	RPGItem = nullptr;
	ItemCount = 0;


}

ULSRPGInventoryComponent::ULSRPGInventoryComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	InventoryItems.AddDefaulted(30);

	SetIsReplicatedByDefault(true);

}

void ULSRPGInventoryComponent::OnRep_InventoryItems()
{
	OnInventoryItemChanged.ExecuteIfBound(InventoryItems);
}

void ULSRPGInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

void ULSRPGInventoryComponent::ActiveSkillByInventoryId(int32 InInventoryId)
{
	if (InventoryItems[InInventoryId].IsValid())
	{
		if (ULSRPGPotion* InPotion = Cast<ULSRPGPotion>(InventoryItems[InInventoryId].RPGItem))
		{
			UAbilitySystemComponent* AbilitySystemComponent = Cast<ALSRPGCharacterBase>(GetOwner())->GetAbilitySystemComponent();

			ULSRPGGameplayAbility* InGA = Cast<ULSRPGGameplayAbility>(InPotion->GrantedAbility->GetDefaultObject());

			FGameplayAbilitySpecHandle  Handle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(InGA));

			AbilitySystemComponent->TryActivateAbility(Handle);

			//自动移除handle
			AbilitySystemComponent->SetRemoveAbilityOnEnd(Handle);

		}
		else if (ULSRPGEquipment* InEquipment = Cast<ULSRPGEquipment>(InventoryItems[InInventoryId].RPGItem))
		{
			ULSRPGEquipmentComponent* EquipmentComponent =
			Cast<ULSRPGEquipmentComponent>(GetOwner()->FindComponentByClass(ULSRPGEquipmentComponent::StaticClass()));


			//通知服务器装备装备
			FLSRPGEquipmentItem InEquipmentItem;
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

void ULSRPGInventoryComponent::CallServerDownLoadInfo()
{
	InventoryItemChanged(InventoryItems);
}

void ULSRPGInventoryComponent::InventoryItemChanged_Implementation(const TArray<FLSRPGInventoryItem>& InInventoryItems)
{
	OnInventoryItemChanged.ExecuteIfBound(InventoryItems);
}

bool ULSRPGInventoryComponent::AddInventoryItem(FLSRPGInventoryItem NewItem)
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

bool ULSRPGInventoryComponent::HasThisInventoryItem(FLSRPGInventoryItem NewItem)
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

bool ULSRPGInventoryComponent::ReplaceInventoryItem(FLSRPGInventoryItem NewItem, int32 InReplace_ID)
{
	if (HasAuthority())
	{
		InventoryItems[InReplace_ID] = NewItem;

		return true;
	}

	return false;
}

void ULSRPGInventoryComponent::RemoveInventoryItem(int32 Index_Remove)
{
	if (HasAuthority())
	{

		InventoryItems[Index_Remove].ResetSelf();


	}
}

void ULSRPGInventoryComponent::RemoveInventoryItemNyItem(FLSRPGInventoryItem NewItem)
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

void ULSRPGInventoryComponent::SwapInventoryItem(int32 Index_i, int32 Index_j)
{
	if (HasAuthority())
	{
		InventoryItems.Swap(Index_i, Index_j);
	}
}

ULSRPGItem* ULSRPGInventoryComponent::GetInventoryItemByID(int32 i)
{

	if (!InventoryItems[i].IsValid())
	{

		return nullptr;
	}



	return InventoryItems[i].RPGItem;

}

FLSRPGInventoryItem ULSRPGInventoryComponent::GetInventoryItemInfoByID(int32 i)
{

	return InventoryItems[i];

}

void ULSRPGInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULSRPGInventoryComponent, InventoryItems);
}
