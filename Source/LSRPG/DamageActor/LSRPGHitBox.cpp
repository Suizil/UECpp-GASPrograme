#include "LSRPGHitBox.h"

#include "Components/BoxComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "LSRPGCharacterBase.h"
#include "Abilities/GameplayAbilityTypes.h"




ALSRPGHitBox::ALSRPGHitBox(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	HitCollisionRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CollisionRootComponent"));
	RootComponent = HitCollisionRootComponent;

	HitDamage = CreateDefaultSubobject<UBoxComponent>((TEXT("HitDamage")));
	HitDamage->SetupAttachment(RootComponent);

	InitialLifeSpan = 4.f;
	bNetLoadOnClient = false;
	bReplicates = false;

}

void ALSRPGHitBox::HandleDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//首先排除自己  在通知攻击函数里面传入， 是谁生成了它
	if (GetInstigator() != OtherActor)
	{
		if (ALSRPGCharacterBase* InPawn = Cast<ALSRPGCharacterBase>(GetInstigator()))
		{
			if (ALSRPGCharacterBase* InTarget = Cast<ALSRPGCharacterBase>(OtherActor))
			{
				//必须在服务器
				if (!InPawn->IsNetMode(ENetMode::NM_Client))
				{				
					//传给GAS的事件数据
					FGameplayEventData EventData;
					EventData.Instigator = GetInstigator();
					EventData.Target = InTarget;

					//已经对该对象产生伤害
					if (IsExist(InTarget))
					{
						return;
					}

					if (!BuffsTags.IsEmpty()) //BuffTags
					{
						for (auto& Tmp : BuffsTags)
						{

							//数据上的处理 受伤 //激活GA的命中事件
							UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), Tmp.Last(), EventData);

						}
					}




					//标记为已处理
					AttackedTarget.AddUnique(InTarget);


				}
			}
		}
	}
}

UPrimitiveComponent* ALSRPGHitBox::GetHitDamage()
{
	return HitDamage;
}

void ALSRPGHitBox::SetHitDamageRelativePosition(const FVector& InNewPostion)
{
	if (UPrimitiveComponent* InHitComponent = GetHitDamage())
	{
		InHitComponent->SetRelativeLocation(InNewPostion);
	}
}

void ALSRPGHitBox::SetBoxExtent(const FVector& InNewBoxExtent)
{
	HitDamage->SetBoxExtent(InNewBoxExtent);
}

bool ALSRPGHitBox::IsExist(ALSRPGCharacterBase* InNewTaget) const
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

void ALSRPGHitBox::BeginPlay()
{
	Super::BeginPlay();

	if (UPrimitiveComponent* InHitComponent = GetHitDamage())
	{
		InHitComponent->SetHiddenInGame(true);
		InHitComponent->OnComponentBeginOverlap.AddDynamic(this, &ALSRPGHitBox::HandleDamage);

	}


}

void ALSRPGHitBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


