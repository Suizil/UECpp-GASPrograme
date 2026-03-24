
#include "UI_DamageNum.h"

#include "Components\TextBlock.h"

void UUI_DamgeNumber::NativeConstruct()
{
	PlayWidgetAnim(TEXT("FadeAnimation"));
}


void UUI_DamgeNumber::UpdateNum(const FText& InText)
{
	if (NumTex)
	{
		NumTex->SetText(InText);
	}



}


void UUI_DamgeNumber::UpdateNumColor(const FLinearColor& InColor)
{
	if (NumTex)
	{
		NumTex->SetColorAndOpacity(FSlateColor(InColor));
	}
}

