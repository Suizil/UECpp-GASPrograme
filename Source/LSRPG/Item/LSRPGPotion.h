#pragma once


#include "CoreMinimal.h"

#include "LSRPGItem.h"
#include "LSRPGPotion.generated.h"


class ULSRPGGameplayAbility;

UCLASS()
class LSRPG_API ULSRPGPotion : public ULSRPGItem
{
	GENERATED_BODY()
public:
	/** Constructor */
	ULSRPGPotion();

public:
	/** Ability to grant if this item is slotted */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TSubclassOf<ULSRPGGameplayAbility> GrantedAbility;

	/** Ability level this item grants. <= 0 means the character level */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	int32 AbilityLevel;



};