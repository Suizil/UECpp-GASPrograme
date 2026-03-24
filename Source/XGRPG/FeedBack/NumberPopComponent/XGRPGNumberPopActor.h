#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"


#include "XGRPGNumberPopActor.generated.h"

class UWidgetComponent;



UCLASS()
class XGRPG_API AXGRPGDamageNumActor : public AActor
{
	GENERATED_BODY()

	//根组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitCollision", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DamageNumRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* DamgeWidgetComponent;


public:
	// Sets default values for this actor's properties
	AXGRPGDamageNumActor();

	void	UpdateNum(const FText& InText);
	void	UpdateNumColor(const FLinearColor& InColor);

};