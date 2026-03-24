

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataAsset.h"
#include "Engine/Texture2D.h"


#include "XGRPGItem.generated.h"


/**
 *
 */
 /** Base class for all items, do not blueprint directly */
UCLASS(Abstract, BlueprintType)
class XGRPG_API UXGRPGItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** Constructor */
	UXGRPGItem()
		: Price(0)
		, MaxCount(1)
		, MaxLevel(1)
	{}


	/** Type of this item, set in native parent class */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	FPrimaryAssetType ItemType;


	/** User-visible short name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemName;

	/** User-visible long description */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemDescription;

	/** Icon to display */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	UTexture2D* ItemIcon;


	/** Price in game */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	int32 Price;


	/** Maximum number of instances that can be in inventory at once, <= 0 means infinite */
	//装备数量为1
	//物品数量可设定具体值>0
	//-1或0 则为无穷大
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
	int32 MaxCount;

	/** Returns if the item is consumable (MaxCount <= 0)*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Max)
	bool IsConsumable() const;


	/** Maximum level this item can be, <= 0 means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
	int32 MaxLevel;

	/** Returns the logical name, equivalent to the primary asset id */
	UFUNCTION(BlueprintCallable, Category = Item)
	FString GetIdentifierString() const;

	/** Overridden to use saved type */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;



};