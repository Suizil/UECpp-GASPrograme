#pragma once


#include "Components/ControllerComponent.h"
#include "GameplayTagContainer.h"
#include "XGRPGNumberPopComponent.h"

#include "XGRPGNumberPopComponent_UMG.generated.h"

class AXGRPGDamageNumActor;

UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class UXGRPGNumberPopComponent_UMG : public UXGRPGNumberPopComponent
{
	GENERATED_BODY()

public:

	UXGRPGNumberPopComponent_UMG(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Adds a damage number to the damage number list for visualization */
	virtual void AddNumberPop(const FXGRPGNumberPopRequest& NewRequest);

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "XGRPG|DamgeNum", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AXGRPGDamageNumActor> NumPopActorClass;



};