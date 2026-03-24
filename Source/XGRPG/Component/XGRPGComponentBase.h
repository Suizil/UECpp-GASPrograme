#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/GameFrameworkComponent.h"

#include "XGRPGComponentBase.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class XGRPG_API UXGRPGComponentBase : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:

	// Sets default values for this component's properties
	UXGRPGComponentBase(const FObjectInitializer& ObjectInitializer);

};