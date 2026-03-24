#pragma once

#include "CoreMinimal.h"

#include "UI_SlotBase.h"

#include "Input/Reply.h"

#include "UI_InventorySlot.generated.h"

class UTextBlock;

/**
 *
 */
UCLASS()
class XGRPG_API UUI_InventorySlot : public UUI_SlotBase
{
	GENERATED_BODY()

	//拖拽显示图标类
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<class UUI_ICONDragDrog> ICODragDrogClass;


public:

	UUI_InventorySlot(const FObjectInitializer& ObjectInitializer);


public:

	UPROPERTY()
	int32  InventoryId;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Num;


public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;



public:

	void HideInventoryNum();

	void ShowInventoryNum();



public:

	//当激活时触发，由蓝图配置映射输入
	virtual void OnClickedWidget() override;



};