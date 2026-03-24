
#include "UI_GameMain.h"


#include "Inventory/UI_InventorySlot.h"
#include "Blueprint/DragDropOperation.h"

#include "Inventory/UI_InventoryPanel.h"

#include "Equipment/UI_EquipmentSlot.h"
#include "Equipment/UI_EquipmentPanel.h"



//防止优化
#if PLATFORM_WINDOWS
#pragma optimize("",off) 
#endif

UUI_GameMain::UUI_GameMain(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void UUI_GameMain::NativeConstruct()
{

	Super::NativeConstruct();
}

void UUI_GameMain::NativeDestruct()
{
	Super::NativeDestruct();




}

void UUI_GameMain::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);


}



bool UUI_GameMain::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	bool bDrop = false;
	//拖拽完成了
	if (InOperation)
	{
		if (AXGRPGCharacterBase* InCharacter = UI_GetXGRPGCharacterBase())
		{
			if (UUI_InventorySlot* MyInventorySlot = Cast<UUI_InventorySlot>(InOperation->Payload))
			{
				MyInventorySlot->ShowSlotIcon();

				MyInventorySlot->ShowInventoryNum();

				bDrop = true;

			}
			else if (UUI_EquipmentSlot* MyEquipmentSlot = Cast<UUI_EquipmentSlot>(InOperation->Payload))
			{
				MyEquipmentSlot->ShowSlotIcon();
				bDrop = true;
			}




		}


	}




	return bDrop;
}


#if PLATFORM_WINDOWS
#pragma optimize("",on) 
#endif