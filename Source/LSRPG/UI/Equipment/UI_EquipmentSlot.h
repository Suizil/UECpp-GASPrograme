#pragma once

#include "CoreMinimal.h"
#include "UI_SlotBase.h"


#include "UI_EquipmentSlot.generated.h"

/**
 *
 */
UCLASS()
class LSRPG_API UUI_EquipmentSlot : public UUI_SlotBase
{
	GENERATED_BODY()

	//拖拽显示图标类
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<class UUI_ICONDragDrog> ICODragDrogClass;

public:

	UUI_EquipmentSlot(const FObjectInitializer& ObjectInitializer);


public:
	UPROPERTY()
	int32  EquipmentId;


	virtual void OnClickedWidget()  override;



public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;


};