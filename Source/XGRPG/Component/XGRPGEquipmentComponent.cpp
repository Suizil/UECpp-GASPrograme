#include "XGRPGEquipmentComponent.h"


#include "XGRPGPotion.h"
#include "XGRPGEquipment.h"
#include "Net/UnrealNetwork.h"

#include "AbilitySystemComponent.h"
#include "XGRPGGameplayAbility.h"
#include "XGRPGCharacterBase.h"

#include "XGRPGInventoryComponent.h"


FXGRPGEquipmentItem::FXGRPGEquipmentItem()
	:RPGEquimentItemPointer(nullptr)
{
}

bool FXGRPGEquipmentItem::IsValid() const
{
	if (RPGEquimentItemPointer)
	{
		return true;
	}
	return false;
}

void FXGRPGEquipmentItem::ResetSelf()
{
	//释放原先的GE 如果有的话
	ActiveEquipmentEffectHandle.Invalidate();
	RPGEquimentItemPointer = nullptr;

}

UXGRPGEquipmentComponent::UXGRPGEquipmentComponent(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

	EquipmentItems.AddDefaulted(6);

	SetIsReplicatedByDefault(true);

}

void UXGRPGEquipmentComponent::OnRep_EquipmentItems()
{
	OnEquipmentItemChanged.ExecuteIfBound(EquipmentItems);



}

void UXGRPGEquipmentComponent::CallServerDownLoadInfo()
{
	EquipmentItemChanged(EquipmentItems);

}

void UXGRPGEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UXGRPGEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UXGRPGEquipmentComponent::AddEquipmentItem(const FXGRPGEquipmentItem& NewEquipmentItem)
{
	UAbilitySystemComponent* AbilitySystemComponent = Cast<AXGRPGCharacterBase>(GetOwner())->GetAbilitySystemComponent();

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

void UXGRPGEquipmentComponent::UndockEquipmentByID(int32 InUndockEquipmentID)
{
	//必须在服务器上调用!!!!!!
	if (HasAuthority())
	{
	UXGRPGInventoryComponent* InventoryComponent = Cast<UXGRPGInventoryComponent>(GetOwner()->FindComponentByClass(UXGRPGInventoryComponent::StaticClass()));

	//1.拿到装备实例

	if (EquipmentItems[InUndockEquipmentID].IsValid())
	{
		FXGRPGEquipmentItem* UndockEquipment = &EquipmentItems[InUndockEquipmentID];

		//2.尝试添加到仓库组件
		FXGRPGInventoryItem AddInventoryItem;
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

bool UXGRPGEquipmentComponent::RemoveEquipmentItem(int32 Index_Remove)
{
	UAbilitySystemComponent* AbilitySystemComponent = Cast<AXGRPGCharacterBase>(GetOwner())->GetAbilitySystemComponent();
	if (FXGRPGEquipmentItem* UndockEquipment = &EquipmentItems[Index_Remove])
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

void UXGRPGEquipmentComponent::SwapEquipmentItem(int32 Index_i, int32 Index_j)
{
	if (Index_i != Index_j && Index_i > INDEX_NONE && Index_i<EquipmentItems.Num() && Index_j>INDEX_NONE && Index_j < EquipmentItems.Num())
	{
		EquipmentItems.Swap(Index_i, Index_j);

		EquipmentItemChanged(EquipmentItems);
	}

}

void UXGRPGEquipmentComponent::SwapFromInvToEqu(int32 InInventory_Index, int32 InEquipment_Index)
{
	//已经在服务器上了,由人物RPC进行调用
	UAbilitySystemComponent* AbilitySystemComponent = Cast<AXGRPGCharacterBase>(GetOwner())->GetAbilitySystemComponent();
	UXGRPGInventoryComponent* InventoryComponent = Cast<UXGRPGInventoryComponent>(GetOwner()->FindComponentByClass(UXGRPGInventoryComponent::StaticClass()));


	//1 还是要判断是不是仓库里面的武器在交互,客户端数据不可信
	if (UXGRPGItem* InRPGItem = InventoryComponent->GetInventoryItemByID(InInventory_Index))
	{
		if (Cast<UXGRPGEquipment>(InRPGItem))
		{
			//2 已经判定好了 执行交换
		
			//a  先准备好要装入的新装备和物品
			FXGRPGEquipmentItem NewEquipmentItem;
			FXGRPGInventoryItem NewInventoryItem;

			//b完成 赋值操作
			NewEquipmentItem.RPGEquimentItemPointer = Cast<UXGRPGEquipment>(InRPGItem);

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

void UXGRPGEquipmentComponent::SwapFromEquToInv(int32 InEquipment_Index, int32 InInventory_Index)
{
	UAbilitySystemComponent* AbilitySystemComponent = Cast<AXGRPGCharacterBase>(GetOwner())->GetAbilitySystemComponent();
	UXGRPGInventoryComponent* InventoryComponent = Cast<UXGRPGInventoryComponent>(GetOwner()->FindComponentByClass(UXGRPGInventoryComponent::StaticClass()));

	//已经在服务器上了,由人物RPC进行调用


	//1 还是要判断是不是仓库里面的武器在交互,客户端数据不可信
	//如果仓库是空的?

	//2 已经判定好了 执行交换

	//a  先准备好要装入的新装备和物品
	FXGRPGEquipmentItem NewEquipmentItem;
	FXGRPGInventoryItem NewInventoryItem;
	bool Equip = false;

	//b完成 赋值操作
	if (UXGRPGItem* InRPGItem = InventoryComponent->GetInventoryItemByID(InInventory_Index))
	{

		if (Cast<UXGRPGPotion>(InRPGItem))
		{
			return;
		}

		if (Cast<UXGRPGEquipment>(InRPGItem))
		{
			NewEquipmentItem.RPGEquimentItemPointer = Cast<UXGRPGEquipment>(InRPGItem);
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

void UXGRPGEquipmentComponent::EquipmentItemChanged_Implementation(const TArray<FXGRPGEquipmentItem>& InEquipmentItems)
{

	OnEquipmentItemChanged.ExecuteIfBound(InEquipmentItems);
}

void UXGRPGEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UXGRPGEquipmentComponent, EquipmentItems);
}
