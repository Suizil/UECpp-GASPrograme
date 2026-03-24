#include "AnimNotify_ApplyGameplayEffect.h"

#include "XGRPGHitBox_ApplyGameEffect.h"
#include "XGRPGCharacterBase.h"


UAnimNotify_ApplyGameplayEffect::UAnimNotify_ApplyGameplayEffect()
	:Super()
{
	HitObjectClass = AXGRPGHitBox_ApplyGameEffect::StaticClass();
	BoxExtent = FVector(32.f);
	LifeTime = 4.0f;
	//去骨骼里面添加这个插槽
	InSocketName = TEXT("OpenFire");



}

FString UAnimNotify_ApplyGameplayEffect::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_ApplyGameplayEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AActor* InCharacter = Cast<AActor>(MeshComp->GetOuter()))
	{
		if (!(InCharacter->GetWorld()->GetNetMode() == ENetMode::NM_Client))
		{

			FVector ComponentLocation = MeshComp->GetSocketLocation(InSocketName);
			FRotator ComponentRotation = MeshComp->GetSocketRotation(InSocketName);


			AXGRPGCharacterBase* CheckChatacter = Cast<AXGRPGCharacterBase>(InCharacter);

			if (!CheckChatacter)
			{
				ComponentRotation += FRotator(0, 90, 0);
			}

			//有待探讨  四元锁 欧拉角的问题
			FTransform Transform((ComponentRotation + RotationOffset).Quaternion(), ComponentLocation);

			FString VForward = ComponentLocation.ToString();

// FString::Printf 格式话字符串
//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, FString::Printf(TEXT(",%s"), *VForward));

			if (AXGRPGHitBox_ApplyGameEffect* HitCollision = InCharacter->GetWorld()->SpawnActorDeferred<AXGRPGHitBox_ApplyGameEffect>(
				HitObjectClass,
				Transform,
				NULL,
				Cast<APawn>(InCharacter),
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
			{

				//延迟生成但是还没有生成 
				// 
				// 		
				//构造体的预初始化 
				HitCollision->PreInitCollision(InCharacter);

				//设置激活的GE类
				HitCollision->SetGameplayEffect(EffectClass);

				HitCollision->SetBoxExtent(BoxExtent);

				//设置相对位置
				FVector RelativeLocation = HitCollision->GetHitDamage()->GetRelativeLocation();
				HitCollision->SetHitDamageRelativePosition(RelativeLocation + RelativeOffsetLocation);

				//设置生命周期
				HitCollision->SetLifeSpan(LifeTime);

				//结束延迟生成
				HitCollision->FinishSpawning(Transform);


				//必须保证对象生成后再附加，否则附加会失败
				if (bBind)
				{
					HitCollision->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, InSocketName);
				}

			}



		}
	}

}
