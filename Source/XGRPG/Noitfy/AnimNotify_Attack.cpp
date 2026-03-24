#include "AnimNotify_Attack.h"
#include "XGRPGHitBox.h"


UAnimNotify_Attack::UAnimNotify_Attack()
{

	HitObjectClass = AXGRPGHitBox::StaticClass();
	BoxExtent = FVector(32.f);
	LifeTime = 4.0f;
	//去骨骼里面添加这个插槽
	InSocketName = TEXT("OpenFire");

}

FString UAnimNotify_Attack::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{

	Super::Notify(MeshComp, Animation, EventReference);

	if (AActor* InCharacter = Cast<AActor>(MeshComp->GetOuter()))
	{

		FVector ComponentLocation = MeshComp->GetSocketLocation(InSocketName);
		FRotator ComponentRotation = MeshComp->GetSocketRotation(InSocketName);

		//只在服务器上生成actor
		if (!InCharacter->GetWorld()->IsNetMode(ENetMode::NM_Client))
		{

			//有待探讨  四元锁 欧拉角的问题
			FTransform Transform((ComponentRotation + RotationOffset).Quaternion(), ComponentLocation);

			if (AXGRPGHitBox* HitCollision = InCharacter->GetWorld()->SpawnActorDeferred<AXGRPGHitBox>(
				HitObjectClass,
				Transform,
				NULL,
				Cast<APawn>(InCharacter),
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
			{

				//延迟生成但是还没有生成 

				//设置激活的目标
				HitCollision->SetBuffs(BuffTags);
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
