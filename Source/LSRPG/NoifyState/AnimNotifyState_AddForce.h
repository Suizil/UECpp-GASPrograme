#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_AddForce.generated.h"


class ACharacter;
UCLASS(meta = (DisplayName = "AddForce"))
class LSRPG_API UAnimNotifyState_AddForce : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_AddForce();


	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotifyState_AddForce")
	FVector DirectionForce;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotifyState_AddForce")
	float ForceSize;


private:
	//总共消耗的时间，就是这个通知的时长
	float TotalDurationConsuming;
	//每次判定消耗后的残值力，当开始时这个力时满值，结束时是0
	float ForceSizeConsuming;

	FVector GetCurrentCharacterDirection(ACharacter* InCharacter);


};