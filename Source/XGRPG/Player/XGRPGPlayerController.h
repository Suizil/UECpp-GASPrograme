#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "XGRPGPlayerController.generated.h"


class AXGRPGPlayerState;
class UXGRPGAbilitySystemComponent;
class AXGRPGHUD;

class UXGRPGNumberPopComponent_UMG;
class UXGRPGNumberPopComponent;

/**
 * AXGRPGPlayerController
 *
 *	The base player controller class used by this project.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base player controller class used by this project."))
class XGRPG_API AXGRPGPlayerController : public APlayerController
{
	GENERATED_BODY()

	//生命组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XGRPG|DamgeNum", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UXGRPGNumberPopComponent_UMG> NumberPopComponent;




public:

	AXGRPGPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	UFUNCTION(BlueprintCallable, Category = "XGRPG|PlayerController")
	AXGRPGPlayerState* GetXGRPGPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "XGRPG|PlayerController")
	UXGRPGAbilitySystemComponent* GetXGRPGAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "XGRPG|PlayerController")
	UXGRPGNumberPopComponent* GetXGRPGNumberPopComponent() const;


	UFUNCTION(BlueprintCallable, Category = "XGRPG|PlayerController")
	AXGRPGHUD* GetXGRPGHUD() const;



	//~APlayerController interface
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	//~End of APlayerController interface


};