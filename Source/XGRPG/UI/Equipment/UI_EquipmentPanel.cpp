
#include "UI_EquipmentPanel.h"

#include "XGRPGCharacterBase.h"
#include "XGRPGEquipmentComponent.h"
#include "UI_EquipmentSlot.h"

#include "Components/Image.h"
#include "XGRPGEquipment.h"


void UUI_EquipmentPanel::NativeConstruct()
{
	Super::NativeConstruct();
	
	//第一次更新创建元素
	if (EquipmentArray->GetChildrenCount() == 0)
	{
		//初始化6个
		for (size_t i = 0; i < 6; i++)
		{

			if (UUI_EquipmentSlot* SlotWidget = CreateWidget<UUI_EquipmentSlot>(GetWorld(), SlotClass))
			{
				SlotWidget->EquipmentId = i;
				if (UWrapBoxSlot* InWrapSlot = EquipmentArray->AddChildToWrapBox(SlotWidget))
				{



				}
			}

		}
	}



	//绑定
	//去角色里面绑定好信息
	if (AXGRPGCharacterBase* InCharacter = UI_GetXGRPGCharacterBase())
	{
		InCharacter->GetXGRPGEquipmentComponent()->OnEquipmentItemChanged.BindUObject(this, &UUI_EquipmentPanel::OnEquipmentItemChanged);
	
		InCharacter->CallServerDownLoadInfo();
	}




}

void UUI_EquipmentPanel::NativeDestruct()
{
	Super::NativeDestruct();
	//移除绑定
	if (AXGRPGCharacterBase* InCharacter = UI_GetXGRPGCharacterBase())
	{
		InCharacter->GetXGRPGEquipmentComponent()->OnEquipmentItemChanged.Unbind();

	}
}

void UUI_EquipmentPanel::OnEquipmentItemChanged(const TArray<FXGRPGEquipmentItem>& InEquipmentItems)
{
	if (InEquipmentItems.Num() > 0)
	{
		for (size_t i = 0; i < 6; i++)
		{

			if (UUI_EquipmentSlot* InSlot = Cast<UUI_EquipmentSlot>(EquipmentArray->GetChildAt(i)))
			{
				//如果数组里面有数据就去更新它
				if (InEquipmentItems[i].IsValid())
				{

					InSlot->SlotIcon->SetBrushFromTexture(InEquipmentItems[i].RPGEquimentItemPointer->ItemIcon);
					InSlot->SlotIcon->SetVisibility(ESlateVisibility::Visible);
				}
				else
				{
					//没有数据就隐藏
					//清空图片的对象
					InSlot->SlotIcon->SetVisibility(ESlateVisibility::Hidden);
					InSlot->SlotIcon->SetBrushResourceObject(NULL);

				}


			}





		}
	}


}

