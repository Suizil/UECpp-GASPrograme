#include "UI_SlotBase.h"

#include "UI_InformationDescribe.h"
#include "Components/Image.h"
#include "Components/Button.h"


UUI_SlotBase::UUI_SlotBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	, SlotType(ESlotType::SlotType_None)
{


}

void UUI_SlotBase::NativeConstruct()
{
	Super::NativeConstruct();

	ClickButton->OnPressed.AddDynamic(this, &UUI_SlotBase::OnClickedWidget);

}

void UUI_SlotBase::HideSlotIcon()
{
	SlotIcon->SetVisibility(ESlateVisibility::Hidden);
}

void UUI_SlotBase::ShowSlotIcon()
{
	SlotIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
}

UWidget* UUI_SlotBase::GetSlotTip()
{
	if (UI_TipClass)
	{
		if (UUI_InformationDescribe* UI_SlotTip = CreateWidget<UUI_InformationDescribe>(GetWorld(), UI_TipClass))
		{
			//初始化tip信息

			return UI_SlotTip;
		}
	}


	return nullptr;
}
