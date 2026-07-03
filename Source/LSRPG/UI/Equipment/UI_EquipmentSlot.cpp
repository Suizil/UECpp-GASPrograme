#include "UI_EquipmentSlot.h"
#include "LSRPGCharacterBase.h"

#include "UI_ICONDragDrog.h"

#include "Blueprint/DragDropOperation.h"

#include "LSRPGCharacterBase.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "UI_InventorySlot.h"

#include "LSRPGItem.h"
#include "LSRPGEquipment.h"

#include "LSRPGInventoryComponent.h"

UUI_EquipmentSlot::UUI_EquipmentSlot(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	SlotType = ESlotType::SlotType_EquipmentSlot;
}

void UUI_EquipmentSlot::OnClickedWidget()
{

	if (ALSRPGCharacterBase* InCharacter = UI_GetLSRPGCharacterBase())
	{

		InCharacter->UndockEquipmentByID(EquipmentId);
	}


}

FReply UUI_EquipmentSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

void UUI_EquipmentSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
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

				HideSlotIcon();
			}

		}

	}

	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);


}

bool UUI_EquipmentSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	//这个地方需要判定你的负载是什么类?>?
	//我们已经确定自己是一个装备栏了
	//那么负载里面的是什么栏 负载栏里面一定是有东西的
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	bool bDrop = false;

	//拖拽完成了
	if (InOperation)
	{
		if (ALSRPGCharacterBase* InCharacter = UI_GetLSRPGCharacterBase())
		{
			//判断负载里面的类型
			if (UUI_SlotBase* InSlotBae = Cast<UUI_SlotBase>(InOperation->Payload))
			{
				ESlotType InSlotType = InSlotBae->GetSlotType();

				switch (InSlotType)
				{
				case ESlotType::SlotType_None:
					check(false);
					break;
				case ESlotType::SlotType_SkillPanelSlot:
					check(false);

					break;
				case ESlotType::SlotType_InventorySlot:

					//判断拖过来的是不是一个装备 还是一个药剂
					//从仓库拖取到装备啊栏
					if (UUI_InventorySlot* MyInventorySlot = Cast<UUI_InventorySlot>(InSlotBae))
					{
						//A1 先客户端处理本地逻辑
						if (ULSRPGItem* InRPGItem = InCharacter->GetLSRPGInventoryComponent()->GetInventoryItemByID(MyInventorySlot->InventoryId))
						{
							if (Cast<ULSRPGEquipment>(InRPGItem))
							{
								//A2 服务器更新覆盖
								//向服务器发起更新逻辑
								//已经完成客户端判定是两个装备进行交换了

								InCharacter->SwapFromInvToEqu(MyInventorySlot->InventoryId, EquipmentId);

								bDrop = true;
								//本来要做客户端的交互,这里不做了,等待服务器刷新数据过来

								//完整逻辑:客户端判定->客户端渲染->客户端发出申请->服务器响应->客户端逻辑更新覆盖


							}
							else
							{

								//判定不是装备,就不能拖进装备栏
								MyInventorySlot->ShowSlotIcon();
								MyInventorySlot->ShowInventoryNum();
								bDrop = true;

							}

						}

					}
					break;
				case ESlotType::SlotType_EquipmentSlot:
					//确定是装备内部交换了

					if (UUI_EquipmentSlot* InEquipmentSlot = Cast<UUI_EquipmentSlot>(InSlotBae))
					{
						InCharacter->SwapEquipmentItem(InEquipmentSlot->EquipmentId, EquipmentId);
						bDrop = true;

					}


					break;
				default:
					check(false);
					break;
				}






			}
		}

	}








	return bDrop;
}
