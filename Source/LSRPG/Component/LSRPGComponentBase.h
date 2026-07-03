#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/GameFrameworkComponent.h"

#include "LSRPGComponentBase.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LSRPG_API ULSRPGComponentBase : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:

	// Sets default values for this component's properties
	ULSRPGComponentBase(const FObjectInitializer& ObjectInitializer);

};