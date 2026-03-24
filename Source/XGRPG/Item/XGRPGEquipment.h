#pragma once

#include "CoreMinimal.h"
#include "XGRPGItem.h"

#include "XGRPGEquipment.generated.h"

class UGameplayEffect;
/**
 *
 */
UCLASS()
class XGRPG_API UXGRPGEquipment : public UXGRPGItem
{
	GENERATED_BODY()

public:
	/** Constructor */
	UXGRPGEquipment();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

};
