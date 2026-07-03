#pragma once

#include "CoreMinimal.h"
#include "LSRPGComponentBase.h"
#include "LSRPGComboComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LSRPG_API ULSRPGComboComponent : public ULSRPGComponentBase
{
	GENERATED_BODY()


public:

	// Sets default values for this component's properties
	ULSRPGComboComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "LS|Fight|Combo")
	int32 GetComboIndex();

	UFUNCTION(BlueprintCallable, Category = "LS|Fight|Combo")
	UAnimMontage* GetComboAnimMontage();

	UFUNCTION(BlueprintCallable, Category = "LS|Fight|Combo")
	UAnimMontage* GetLastPlayAnimMontage();

	UFUNCTION(BlueprintCallable, Category = "LS|Fight|Combo")
	void UpdateComboIndex();

	UFUNCTION(BlueprintCallable, Category = "LS|Fight|Combo")
	void ResetComboIndex();

	UFUNCTION(BlueprintCallable, Category = "LS|Fight|Combo")
	void SetPressed();

	UFUNCTION(BlueprintCallable, Category = "LS|Fight|Combo")
	void ResetPressed();

	UFUNCTION(BlueprintCallable, Category = "LS|Fight|Combo")
	bool IsStillPressed();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "LS|Fight|Combo")
	TArray<UAnimMontage*> ComboMontages;

	UPROPERTY();
	UAnimMontage* LastPlayMontage = nullptr;

	int32 ComboIndex = 0;

	int32 ComboMaxIndex = 0;

	bool bShortPress = false;


};
