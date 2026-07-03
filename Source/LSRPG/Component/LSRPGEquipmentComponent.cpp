#include "LSRPGEquipmentComponent.h"


#include "LSRPGPotion.h"
#include "LSRPGEquipment.h"
#include "Net/UnrealNetwork.h"

#include "AbilitySystemComponent.h"
#include "LSRPGGameplayAbility.h"
#include "LSRPGCharacterBase.h"

#include "LSRPGInventoryComponent.h"


FLSRPGEquipmentItem::FLSRPGEquipmentItem()
	:RPGEquimentItemPointer(nullptr)
{
}

bool FLSRPGEquipmentItem::IsValid() const
{
	if (RPGEquimentItemPointer)
	{
		return true;
	}
	return false;
}

void FLSRPGEquipmentItem::ResetSelf()
{
	//释放原先的GE 如果有的话
	ActiveEquipmentEffectHandle.Invalidate();
	RPGEquimentItemPointer = nullptr;

}

ULSRPGEquipmentComponent::ULSRPGEquipmentComponent(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

	EquipmentItems.AddDefaulted(6);

	SetIsReplicatedByDefault(true);

}

void ULSRPGEquipmentComponent::OnRep_EquipmentItems()
{
	OnEquipmentItemChanged.ExecuteIfBound(EquipmentItems);



}

void ULSRPGEquipmentComponent::CallServerDownLoadInfo()
{
	EquipmentItemChanged(EquipmentItems);

}

void ULSRPGEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULSRPGEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool ULSRPGEquipmentComponent::AddEquipmentItem(const FLSRPGEquipmentItem& NewEquipmentItem)
{
	UAbilitySystemComponent* AbilitySystemComponent = Cast<ALSRPGCharacterBase>(GetOwner())->GetAbilitySystemComponent();

	//只在服务器操作数据
	if (HasAuthority())
	{

		if (!NewEquipmentItem.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("AddInventoryItem: Failed trying to add null item!"));
			return false;

		}

		//去找一下有米有空的位置
		//1. 往数组里面增加
		for (size_t i = 0; i < EquipmentItems.Num(); i++)
		{
			if (!EquipmentItems[i].IsValid())
			{
				EquipmentItems[i] = NewEquipmentItem;

				//2.应用新的效果

				//创建GEhandle
				//拿到我们的 GAS系统组件
				//创建GE的实例 ，传入阐述， GE本体，GE的等级，GE的上下文信息

				const	FGameplayEffectSpecHandle NewHandle =
					AbilitySystemComponent->MakeOutgoingSpec(EquipmentItems[i].RPGEquimentItemPointer->GameplayEffectClass,
						1,
						AbilitySystemComponent->MakeEffectContext());

				//应用GE 存储激活的效果
				EquipmentItems[i].ActiveEquipmentEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data);

				//EquipmentItemChanged(EquipmentItems);

				return true;

			}

		}
	}


	return false;
}

void ULSRPGEquipmentComponent::UndockEquipmentByID(int32 InUndockEquipmentID)
{
	//必须在服务器上调用!!!!!!
	if (HasAuthority())
	{
	ULSRPGInventoryComponent* InventoryComponent = Cast<ULSRPGInventoryComponent>(GetOwner()->FindComponentByClass(ULSRPGInventoryComponent::StaticClass()));

	//1.拿到装备实例

	if (EquipmentItems[InUndockEquipmentID].IsValid())
	{
		FLSRPGEquipmentItem* UndockEquipment = &EquipmentItems[InUndockEquipmentID];

		//2.尝试添加到仓库组件
		FLSRPGInventoryItem AddInventoryItem;
		AddInventoryItem.RPGItem = UndockEquipment->RPGEquimentItemPointer;
		AddInventoryItem.ItemCount = 1;

		if (InventoryComponent->AddInventoryItem(AddInventoryItem))
		{
			//3.添加成功了,移除自己
			RemoveEquipmentItem(InUndockEquipmentID);
			


		}
	}
	}
}

bool ULSRPGEquipmentComponent::RemoveEquipmentItem(int32 Index_Remove)
{
	UAbilitySystemComponent* AbilitySystemComponent = Cast<ALSRPGCharacterBase>(GetOwner())->GetAbilitySystemComponent();
	if (FLSRPGEquipmentItem* UndockEquipment = &EquipmentItems[Index_Remove])
	{

		if (UndockEquipment->IsValid())
		{

			// 移除buff
			bool	bRemoveGameplayEffect = AbilitySystemComponent->RemoveActiveGameplayEffect(UndockEquipment->ActiveEquipmentEffectHandle);

			if (bRemoveGameplayEffect)
			{
				//重置自己
				UndockEquipment->ResetSelf();
				return true;
			}


		}
	}



	return false;
}

void ULSRPGEquipmentComponent::SwapEquipmentItem(int32 Index_i, int32 Index_j)
{
	if (Index_i != Index_j && Index_i > INDEX_NONE && Index_i<EquipmentItems.Num() && Index_j>INDEX_NONE && Index_j < EquipmentItems.Num())
	{
		EquipmentItems.Swap(Index_i, Index_j);

		EquipmentItemChanged(EquipmentItems);
	}

}

void ULSRPGEquipmentComponent::SwapFromInvToEqu(int32 InInventory_Index, int32 InEquipment_Index)
{
	//已经在服务器上了,由人物RPC进行调用
	UAbilitySystemComponent* AbilitySystemComponent = Cast<ALSRPGCharacterBase>(GetOwner())->GetAbilitySystemComponent();
	ULSRPGInventoryComponent* InventoryComponent = Cast<ULSRPGInventoryComponent>(GetOwner()->FindComponentByClass(ULSRPGInventoryComponent::StaticClass()));


	//1 还是要判断是不是仓库里面的武器在交互,客户端数据不可信
	if (ULSRPGItem* InRPGItem = InventoryComponent->GetInventoryItemByID(InInventory_Index))
	{
		if (Cast<ULSRPGEquipment>(InRPGItem))
		{
			//2 已经判定好了 执行交换
		
			//a  先准备好要装入的新装备和物品
			FLSRPGEquipmentItem NewEquipmentItem;
			FLSRPGInventoryItem NewInventoryItem;

			//b完成 赋值操作
			NewEquipmentItem.RPGEquimentItemPointer = Cast<ULSRPGEquipment>(InRPGItem);

			NewInventoryItem.RPGItem = (EquipmentItems[InEquipment_Index].RPGEquimentItemPointer);
			NewInventoryItem.ItemCount = 1;

			//c 移除装备
			RemoveEquipmentItem(InEquipment_Index);

			//d 添加装备 施加GE

			EquipmentItems[InEquipment_Index] = NewEquipmentItem;

			const	FGameplayEffectSpecHandle NewHandle =
				AbilitySystemComponent->MakeOutgoingSpec(EquipmentItems[InEquipment_Index].RPGEquimentItemPointer->GameplayEffectClass,
					1,
					AbilitySystemComponent->MakeEffectContext());


			//应用GE 存储激活的效果
			EquipmentItems[InEquipment_Index].ActiveEquipmentEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data);


			//e 移除仓库
			//f 添加仓库

			InventoryComponent->ReplaceInventoryItem(NewInventoryItem, InInventory_Index);

			//g	刷新仓库和装备

			CallServerDownLoadInfo();
			InventoryComponent->CallServerDownLoadInfo();



		}
		else
		{
			return;
		}


	}






}

void ULSRPGEquipmentComponent::SwapFromEquToInv(int32 InEquipment_Index, int32 InInventory_Index)
{
	UAbilitySystemComponent* AbilitySystemComponent = Cast<ALSRPGCharacterBase>(GetOwner())->GetAbilitySystemComponent();
	ULSRPGInventoryComponent* InventoryComponent = Cast<ULSRPGInventoryComponent>(GetOwner()->FindComponentByClass(ULSRPGInventoryComponent::StaticClass()));

	//已经在服务器上了,由人物RPC进行调用


	//1 还是要判断是不是仓库里面的武器在交互,客户端数据不可信
	//如果仓库是空的?

	//2 已经判定好了 执行交换

	//a  先准备好要装入的新装备和物品
	FLSRPGEquipmentItem NewEquipmentItem;
	FLSRPGInventoryItem NewInventoryItem;
	bool Equip = false;

	//b完成 赋值操作
	if (ULSRPGItem* InRPGItem = InventoryComponent->GetInventoryItemByID(InInventory_Index))
	{

		if (Cast<ULSRPGPotion>(InRPGItem))
		{
			return;
		}

		if (Cast<ULSRPGEquipment>(InRPGItem))
		{
			NewEquipmentItem.RPGEquimentItemPointer = Cast<ULSRPGEquipment>(InRPGItem);
			Equip = true;
		}
		else
		{
			NewEquipmentItem.RPGEquimentItemPointer = nullptr;
		}
	}
	else
	{
		NewEquipmentItem.RPGEquimentItemPointer = nullptr;
	}

	NewInventoryItem.RPGItem = (EquipmentItems[InEquipment_Index].RPGEquimentItemPointer);
	NewInventoryItem.ItemCount = 1;

	//c 移除装备
	RemoveEquipmentItem(InEquipment_Index);


	//d 添加装备 施加GE
	EquipmentItems[InEquipment_Index] = NewEquipmentItem;

	if (Equip)
	{
		const	FGameplayEffectSpecHandle NewHandle =
			AbilitySystemComponent->MakeOutgoingSpec(EquipmentItems[InEquipment_Index].RPGEquimentItemPointer->GameplayEffectClass,
				1,
				AbilitySystemComponent->MakeEffectContext());


		//应用GE 存储激活的效果
		EquipmentItems[InEquipment_Index].ActiveEquipmentEffectHandle 
		= AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data);
	}

	//e 移除仓库
	//f 添加仓库

	InventoryComponent->ReplaceInventoryItem(NewInventoryItem, InInventory_Index);

	//g	刷新仓库和装备
	CallServerDownLoadInfo();
	InventoryComponent->CallServerDownLoadInfo();

}

void ULSRPGEquipmentComponent::EquipmentItemChanged_Implementation(const TArray<FLSRPGEquipmentItem>& InEquipmentItems)
{

	OnEquipmentItemChanged.ExecuteIfBound(InEquipmentItems);
}

void ULSRPGEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ULSRPGEquipmentComponent, EquipmentItems);
}
