#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "LSRPGPlayerController.generated.h"


class ALSRPGPlayerState;
class ULSRPGAbilitySystemComponent;
class ALSRPGHUD;

class ULSRPGNumberPopComponent_UMG;
class ULSRPGNumberPopComponent;

/**
 * ALSRPGPlayerController
 *
 *	The base player controller class used by this project.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base player controller class used by this project."))
class LSRPG_API ALSRPGPlayerController : public APlayerController
{
	GENERATED_BODY()

	//生命组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LSRPG|DamgeNum", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULSRPGNumberPopComponent_UMG> NumberPopComponent;




public:

	ALSRPGPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	UFUNCTION(BlueprintCallable, Category = "LSRPG|PlayerController")
	ALSRPGPlayerState* GetLSRPGPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "LSRPG|PlayerController")
	ULSRPGAbilitySystemComponent* GetLSRPGAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "LSRPG|PlayerController")
	ULSRPGNumberPopComponent* GetLSRPGNumberPopComponent() const;


	UFUNCTION(BlueprintCallable, Category = "LSRPG|PlayerController")
	ALSRPGHUD* GetLSRPGHUD() const;



	//~APlayerController interface
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	//~End of APlayerController interface


};