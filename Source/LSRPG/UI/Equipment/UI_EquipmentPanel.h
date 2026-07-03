#pragma once

#include "CoreMinimal.h"

#include "LSRPGEquipmentComponent.h"
#include "Components/WrapBox.h"
#include "UI_Base.h"

#include "UI_EquipmentPanel.generated.h"

class UUI_EquipmentSlot;

/**
 *
 */
UCLASS()
class LSRPG_API UUI_EquipmentPanel : public UUI_Base
{
	GENERATED_BODY()

	//框框 用来装元素 类似于水平框 竖直框
	UPROPERTY(meta = (BindWidget))
	UWrapBox* EquipmentArray;

	//蓝图配置
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUI_EquipmentSlot> SlotClass;


public:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;



	//用来接受装备组件发来的 信息
	UFUNCTION()
	void OnEquipmentItemChanged(const TArray<FLSRPGEquipmentItem>& InEquipmentItems);


};