#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_DamageNum.generated.h"

/**
 *
 */
class UTextBlock;

UCLASS()
class XGRPG_API UUI_DamgeNumber : public UUI_Base
{

	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NumTex;

public:
	virtual void NativeConstruct();

	void	UpdateNum(const FText& InText);
	void	UpdateNumColor(const FLinearColor& InColor);


};