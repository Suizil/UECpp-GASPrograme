#pragma once

#include "CoreMinimal.h"
#include "XGRPGComponentBase.h"
#include "XGRPGComboComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class XGRPG_API UXGRPGComboComponent : public UXGRPGComponentBase
{
	GENERATED_BODY()


public:

	// Sets default values for this component's properties
	UXGRPGComboComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "XG|Fight|Combo")
	int32 GetComboIndex();

	UFUNCTION(BlueprintCallable, Category = "XG|Fight|Combo")
	UAnimMontage* GetComboAnimMontage();

	UFUNCTION(BlueprintCallable, Category = "XG|Fight|Combo")
	UAnimMontage* GetLastPlayAnimMontage();

	UFUNCTION(BlueprintCallable, Category = "XG|Fight|Combo")
	void UpdateComboIndex();

	UFUNCTION(BlueprintCallable, Category = "XG|Fight|Combo")
	void ResetComboIndex();

	UFUNCTION(BlueprintCallable, Category = "XG|Fight|Combo")
	void SetPressed();

	UFUNCTION(BlueprintCallable, Category = "XG|Fight|Combo")
	void ResetPressed();

	UFUNCTION(BlueprintCallable, Category = "XG|Fight|Combo")
	bool IsStillPressed();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "XG|Fight|Combo")
	TArray<UAnimMontage*> ComboMontages;

	UPROPERTY();
	UAnimMontage* LastPlayMontage = nullptr;

	int32 ComboIndex = 0;

	int32 ComboMaxIndex = 0;

	bool bShortPress = false;


};
