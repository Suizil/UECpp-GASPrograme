#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"

#include "Components/UniformGridPanel.h"
#include "LSRPGInventoryComponent.h"

#include "UI_InventoryPanel.generated.h"

class UUI_InventorySlot;


UCLASS()
class LSRPG_API UUI_InventoryPanel : public UUI_Base
{
	GENERATED_BODY()


	//框框 用来装元素 类似于水平框 竖直框
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* InventoryArray;

	//蓝图配置
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUI_InventorySlot> InventoryClass;


public:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
	//Tick
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	//用来接受仓库组件发来的 信息
	UFUNCTION()
	void OnInventoryItemChanged(const TArray<FLSRPGInventoryItem>& InInventoryItems);


};