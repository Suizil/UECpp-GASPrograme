
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "XGRPGHitBox_ApplyGameEffect.generated.h"


class AXGRPGCharacterBase;
class UProjectileMovementComponent;


//这是一个定义碰撞类型的枚举
UENUM(BlueprintType)
enum class EXGRPGHitCollisionType :uint8
{
	HITCOLLISIONTYPE_SHORT_RANGE_ATTACK UMETA(DisplayName = "Short range attack"),		//近程攻击

	HITCOLLISIONTYPE_DIRECT_LINE 		UMETA(DisplayName = "Direct Line"),				//无障碍直线攻击
	HITCOLLISIONTYPE_LINE				UMETA(DisplayName = "Line"),					//非跟踪类型，类似手枪子弹；
	HITCOLLISIONTYPE_TRACK_LINE			UMETA(DisplayName = "Track Line"),				//跟踪类型
	HITCOLLISIONTYPE_RANGE_LINE			UMETA(DisplayName = "Range Line"),				//范围伤害，丢手雷；
	HITCOLLISIONTYPE_RANGE				UMETA(DisplayName = "Range"),					//范围伤害，类似自爆；
	HITCOLLISIONTYPE_CHAIN				UMETA(DisplayName = "Chain"),					//链条类型，持续伤害类型;
};


UCLASS(BlueprintType, Blueprintable)
class XGRPG_API AXGRPGHitBox_ApplyGameEffect : public AActor
{
	GENERATED_BODY()
	

	//根组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitCollision", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* HitCollisionRootComponent;

	//碰撞盒子
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttribute", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* HitDamage;

	//投掷组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitCollision", meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;


public:
	//碰撞体的类型
	UPROPERTY(EditDefaultsOnly, Category = "HitCollision")
	EXGRPGHitCollisionType HitCollisionType;


public:
	// Sets default values for this actor's properties
	AXGRPGHitBox_ApplyGameEffect(const FObjectInitializer& ObjectInitializer);

	//这个是用来在初始化之前设置碰撞类型的函数
	virtual void PreInitCollision(AActor* InMyInstigator);


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
	/**
	 *	@brief  设置盒子的位置
	 *	@param  InNewPostion 新的位置
	 *	......
	 *	@return  ;
	 */
	void SetHitDamageRelativePosition(const FVector& InNewPostion);
	/**  单行文本注释*/
	//
	// 
	// 
	//设置盒子大小
	/**
	 * @brief
	 * @param
	 * @return
	 */
	void SetBoxExtent(const FVector& InNewBoxExtent);

	//设置Gas标签，告诉系统，我们要激发哪一个对应的能力
	void SetGameplayEffect(TSubclassOf< UGameplayEffect>& InGE) {
		EffectClass = InGE;
	};



	//是否意见造成效果
	bool IsExist(AXGRPGCharacterBase* InNewTaget) const;


protected:
	// Called when the game starts or when 

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY()
	TSubclassOf < UGameplayEffect> EffectClass;


	//储存我们的重叠目标
	UPROPERTY()
	TArray<TWeakObjectPtr<AXGRPGCharacterBase>> AttackedTarget;

};