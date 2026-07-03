#pragma once



#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

#include "LSRPGHitBox.generated.h"


class ALSRPGCharacterBase;


// Blueprintable 可以被蓝图类继承，该说明符会传递给派生类
//BlueprintType 可用于蓝图的变量类型
//DisplayName="Blueprint Node Name"	蓝图中的节点名
UCLASS(BlueprintType, Blueprintable)
class LSRPG_API ALSRPGHitBox : public AActor
{
	GENERATED_BODY()


	//根组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitCollision", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* HitCollisionRootComponent;

	//碰撞盒子
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttribute", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* HitDamage;

public:
	// Sets default values for this actor's properties
	ALSRPGHitBox(const FObjectInitializer& ObjectInitializer);

	//执行碰撞效果的函数
	UFUNCTION()
	virtual void HandleDamage(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);


	//返回碰撞盒子的函数
	virtual UPrimitiveComponent* GetHitDamage();

	//设置碰撞盒子位置的函数
	void SetHitDamageRelativePosition(const FVector& InNewPostion);

	//设置盒子大小
	void SetBoxExtent(const FVector& InNewBoxExtent);

	//设置Gas标签，告诉系统，我们要激发哪一个对应的能力
	void SetBuffs(const TArray<FGameplayTagContainer>& InBuffs) { BuffsTags = InBuffs; };

	//是否意见造成效果
	bool IsExist(ALSRPGCharacterBase* InNewTaget) const;

protected:
	// Called when the game starts or when 

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;



protected:
	UPROPERTY()
	TArray<FGameplayTagContainer> BuffsTags;

	//储存我们的重叠目标
	UPROPERTY()
	TArray<TWeakObjectPtr<ALSRPGCharacterBase>> AttackedTarget;



};
