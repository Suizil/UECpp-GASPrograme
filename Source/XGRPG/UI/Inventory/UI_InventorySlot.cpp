#include "UI_InventorySlot.h"

#include "UI_ICONDragDrog.h"

#include "Blueprint/DragDropOperation.h"

#include "XGRPGCharacterBase.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "UI_EquipmentSlot.h"
#include "XGRPGEquipment.h"

#include "XGRPGInventoryComponent.h"
#include "XGRPGEquipmentComponent.h"



UUI_InventorySlot::UUI_InventorySlot(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

	SlotType = ESlotType::SlotType_InventorySlot;
}

FReply UUI_InventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton || InMouseEvent.IsTouchEvent())
	{
		FReply Reply = FReply::Handled();

		TSharedPtr<SWidget> SlateWidgetDrag = GetCachedWidget();

		if (SlateWidgetDrag.IsValid())
		{
			Reply.DetectDrag(SlateWidgetDrag.ToSharedRef(), EKeys::RightMouseButton);
			return Reply;
		}



	}


	return FReply::Handled();
}

void UUI_InventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (ICODragDrogClass && SlotIcon && SlotIcon->Brush.GetResourceObject())
	{

		if (UUI_ICONDragDrog* ICODragDrog = CreateWidget<UUI_ICONDragDrog>(GetWorld(), ICODragDrogClass))
		{

			//可以自定义的
			if (UDragDropOperation* InDropOperation = NewObject<UDragDropOperation>(GetTransientPackage(), UDragDropOperation::StaticClass()))
			{

				//给一个强应用 避免GC
				//InDropOperation->SetFlags(RF_StrongRefOnFrame);

				//图片赋值
				ICODragDrog->DrawICON(Cast<UTexture2D>(SlotIcon->Brush.GetResourceObject()));


				//定位 UE帮我们写好了 定位鼠标的位置进行渲染
				InDropOperation->DefaultDragVisual = ICODragDrog;

				//传入我们的负载信息，告知我们从哪里拖过来的，从哪里的
				InDropOperation->Payload = this;
				OutOperation = InDropOperation;
				HideInventoryNum();
				HideSlotIcon();
			}

		}
	}


	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool UUI_InventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	//我们现在已经确定我们自己是仓库了
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	bool bDrop = false;

	//拖拽完成了
	if (InOperation)
	{
		if (AXGRPGCharacterBase* InCharacter = UI_GetXGRPGCharacterBase())
		{
			//判断负载里面的类型
			if (UUI_SlotBase* InSlotBae = Cast<UUI_SlotBase>(InOperation->Payload))
			{

				ESlotType InSlotType = InSlotBae->GetSlotType();

				switch (InSlotType)
				{
				case ESlotType::SlotType_None:
					InSlotBae->ShowSlotIcon();
					bDrop = true;
					break;
				case ESlotType::SlotType_InventorySlot:
					//已经确定了对象是仓库,我们直接呼叫交互
					if (UUI_InventorySlot* MyInventorySlot = Cast<UUI_InventorySlot>(InOperation->Payload))
					{
						//调用我们的角色在服务器上进行仓库内容交换

						InCharacter->SwapInventoryItem(MyInventorySlot->InventoryId, InventoryId);
						bDrop = true;

					}

					break;
				case ESlotType::SlotType_EquipmentSlot:

					//已经确定对象是装备栏了 
					//装备栏肯定是有东西的,没有东西是拖不动的
					if (UUI_EquipmentSlot* MyEquipmentSlot = Cast<UUI_EquipmentSlot>(InSlotBae))
					{
						//A1 先客户端处理本地逻辑
						//注意这里是判定自己的物品类型 而不是负载的物品类型

						//A2 如果我们自己是一个装备,我们就呼叫服务器执行装备交换								
						//如果是空的呢?我们也要呼叫交换	 	

						if (Cast<UXGRPGEquipment>(InCharacter->GetXGRPGInventoryComponent()->GetInventoryItemByID(InventoryId))
							|| !InCharacter->GetXGRPGInventoryComponent()->GetInventoryItemInfoByID(InventoryId).IsValid())
						{

							InCharacter->SwapFromEquToInv(MyEquipmentSlot->EquipmentId, InventoryId);
							bDrop = true;

						}
						else
						{
							//A3 已经确定 我们不是装备,不能进行交换,所以需要完成拖拽操作
							InSlotBae->ShowSlotIcon();
							bDrop = true;
						}





					}

					break;
				default:
					break;
				}


			}
		}
	}
	return bDrop;
}

void UUI_InventorySlot::HideInventoryNum()
{
	Num->SetVisibility(ESlateVisibility::Hidden);
}

void UUI_InventorySlot::ShowInventoryNum()
{
	Num->SetVisibility(ESlateVisibility::HitTestInvisible);

}

void UUI_InventorySlot::OnClickedWidget()
{
	if (AXGRPGCharacterBase* InCharacter = UI_GetXGRPGCharacterBase())
	{
		InCharacter->ActiveSkillByInventoryId(InventoryId);
	}


}
