#pragma once

#include "CoreMinimal.h"
#include "LSRPGItem.h"

#include "LSRPGEquipment.generated.h"

class UGameplayEffect;
/**
 *
 */
UCLASS()
class LSRPG_API ULSRPGEquipment : public ULSRPGItem
{
	GENERATED_BODY()

public:
	/** Constructor */
	ULSRPGEquipment();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

};
