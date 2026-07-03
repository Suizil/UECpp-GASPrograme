#pragma once


#include "Components/ControllerComponent.h"
#include "GameplayTagContainer.h"
#include "LSRPGNumberPopComponent.h"

#include "LSRPGNumberPopComponent_UMG.generated.h"

class ALSRPGDamageNumActor;

UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class ULSRPGNumberPopComponent_UMG : public ULSRPGNumberPopComponent
{
	GENERATED_BODY()

public:

	ULSRPGNumberPopComponent_UMG(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Adds a damage number to the damage number list for visualization */
	virtual void AddNumberPop(const FLSRPGNumberPopRequest& NewRequest);

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "LSRPG|DamgeNum", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ALSRPGDamageNumActor> NumPopActorClass;



};