#include "LSRPGHitBox_ApplyGameEffect.h"

#include "Components/BoxComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "LSRPGCharacterBase.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "AbilitySystemGlobals.h"

ALSRPGHitBox_ApplyGameEffect::ALSRPGHitBox_ApplyGameEffect(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	HitCollisionRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CollisionRootComponent"));
	RootComponent = HitCollisionRootComponent;

	HitDamage = CreateDefaultSubobject<UBoxComponent>((TEXT("HitDamage")));
	HitDamage->SetupAttachment(RootComponent);

	InitialLifeSpan = 4.f;
	bNetLoadOnClient = true;
	bReplicates = true;
	SetReplicatingMovement(true);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	//最大速度
	ProjectileMovement->MaxSpeed = 2000.f;
	//初始速度
	ProjectileMovement->InitialSpeed = 1600.f;
	//关闭重力 ，不关重力就是抛物线
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->SetUpdatedComponent(RootComponent);

	ProjectileMovement->SetIsReplicated(true);

	// 默认近程攻击
	HitCollisionType = ELSRPGHitCollisionType::HITCOLLISIONTYPE_SHORT_RANGE_ATTACK;


}

void ALSRPGHitBox_ApplyGameEffect::PreInitCollision(AActor* InMyInstigator)
{
	if (!InMyInstigator)
	{
		return;
	}

	FVector ShootDirection = InMyInstigator->GetActorRotation().Vector();


	switch (HitCollisionType)
	{
	case ELSRPGHitCollisionType::HITCOLLISIONTYPE_SHORT_RANGE_ATTACK:

		ProjectileMovement->MaxSpeed = 0.f;
		ProjectileMovement->InitialSpeed = 0.f;
		ProjectileMovement->ProjectileGravityScale = 0.f;

		break;
	case ELSRPGHitCollisionType::HITCOLLISIONTYPE_DIRECT_LINE:

		RootComponent->SetWorldRotation(FRotator::ZeroRotator);
		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;

		break;
	case ELSRPGHitCollisionType::HITCOLLISIONTYPE_LINE:
		break;
	case ELSRPGHitCollisionType::HITCOLLISIONTYPE_TRACK_LINE:
		break;
	case ELSRPGHitCollisionType::HITCOLLISIONTYPE_RANGE_LINE:
		break;
	case ELSRPGHitCollisionType::HITCOLLISIONTYPE_RANGE:
		break;
	case ELSRPGHitCollisionType::HITCOLLISIONTYPE_CHAIN:
		break;
	default:
		break;
	}


}

void ALSRPGHitBox_ApplyGameEffect::HandleDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != GetInstigator())
	{
		if (ALSRPGCharacterBase* InTarget = Cast<ALSRPGCharacterBase>(OtherActor))
		{
			//必须在服务器
			if (!InTarget->IsNetMode(ENetMode::NM_Client))
			{

				//已经对该对象产生伤害
				if (IsExist(InTarget))
				{
					return;
				}

				//拿到一个GAS
				if (UAbilitySystemComponent* InAbilitySystemComponent = InTarget->GetAbilitySystemComponent())
				{

					//创造一个上下文
					FGameplayEffectContextHandle EffectContext = InAbilitySystemComponent->MakeEffectContext();

					EffectContext.AddInstigator(GetInstigator(), this);
					//添加一个源目标
					EffectContext.AddSourceObject(InTarget);


					//创建一个实例GE， 并得到它的handle
					//GE的类，等级（默认参数用来查询等级不同造成不同的效果）
					FGameplayEffectSpecHandle EffectSpecHandle =
						InAbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1, EffectContext);

					if (EffectSpecHandle.IsValid())
					{
						//应用的实例到目标
						InAbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), InAbilitySystemComponent);
					}

					//标记为已处理
					AttackedTarget.AddUnique(InTarget);




				}
			}


		}

	}

}

UPrimitiveComponent* ALSRPGHitBox_ApplyGameEffect::GetHitDamage()
{
	return HitDamage;
}

void ALSRPGHitBox_ApplyGameEffect::SetHitDamageRelativePosition(const FVector& InNewPostion)
{
	if (UPrimitiveComponent* InHitComponent = GetHitDamage())
	{
		InHitComponent->SetRelativeLocation(InNewPostion);
	}
}

void ALSRPGHitBox_ApplyGameEffect::SetBoxExtent(const FVector& InNewBoxExtent)
{
	HitDamage->SetBoxExtent(InNewBoxExtent);
}

bool ALSRPGHitBox_ApplyGameEffect::IsExist(ALSRPGCharacterBase* InNewTaget) const
{
	for (auto& Tmp : AttackedTarget)
	{
		if (Tmp.IsValid())
		{
			if (Tmp == InNewTaget)
			{
				return true;
			}
		}
	}

	return false;
}

void ALSRPGHitBox_ApplyGameEffect::BeginPlay()
{
	Super::BeginPlay();

	if (UPrimitiveComponent* InHitComponent = GetHitDamage())
	{
		InHitComponent->SetHiddenInGame(true);
		InHitComponent->OnComponentBeginOverlap.AddDynamic(this, &ALSRPGHitBox_ApplyGameEffect::HandleDamage);

	}
}

void ALSRPGHitBox_ApplyGameEffect::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

}
