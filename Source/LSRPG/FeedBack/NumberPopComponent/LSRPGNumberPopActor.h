#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"


#include "LSRPGNumberPopActor.generated.h"

class UWidgetComponent;



UCLASS()
class LSRPG_API ALSRPGDamageNumActor : public AActor
{
	GENERATED_BODY()

	//根组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitCollision", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DamageNumRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* DamgeWidgetComponent;


public:
	// Sets default values for this actor's properties
	ALSRPGDamageNumActor();

	void	UpdateNum(const FText& InText);
	void	UpdateNumColor(const FLinearColor& InColor);

};