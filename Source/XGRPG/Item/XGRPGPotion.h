#pragma once


#include "CoreMinimal.h"

#include "XGRPGItem.h"
#include "XGRPGPotion.generated.h"


class UXGRPGGameplayAbility;

UCLASS()
class XGRPG_API UXGRPGPotion : public UXGRPGItem
{
	GENERATED_BODY()
public:
	/** Constructor */
	UXGRPGPotion();

public:
	/** Ability to grant if this item is slotted */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TSubclassOf<UXGRPGGameplayAbility> GrantedAbility;

	/** Ability level this item grants. <= 0 means the character level */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	int32 AbilityLevel;



};