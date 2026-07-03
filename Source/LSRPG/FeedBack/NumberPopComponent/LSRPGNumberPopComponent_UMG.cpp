#include "LSRPGNumberPopComponent_UMG.h"
#include "LSRPGNumberPopActor.h"
ULSRPGNumberPopComponent_UMG::ULSRPGNumberPopComponent_UMG(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{




}

void ULSRPGNumberPopComponent_UMG::AddNumberPop(const FLSRPGNumberPopRequest& NewRequest)
{

	// Drop requests for remote players on the floor
	// (this prevents multiple pops from showing up for the host of a listen server)
	if (APlayerController* PC = GetOwner<APlayerController>())
	{
		if (!PC->IsLocalController())
		{
			return;
		}
	}

	// Determine the position
	FTransform CameraTransform;
	FVector NumberLocation(NewRequest.WorldLocation);

	if (APlayerController* PC = GetOwner<APlayerController>())
	{
		if (APlayerCameraManager* PlayerCameraManager = PC->PlayerCameraManager)
		{
			CameraTransform = FTransform(PlayerCameraManager->GetCameraRotation(), PlayerCameraManager->GetCameraLocation());

			FVector LocationOffset(ForceInitToZero);

			const float RandomMagnitude = 5.0f; //@TODO: Make this style driven
			LocationOffset += FMath::RandPointInBox(FBox(FVector(-RandomMagnitude), FVector(RandomMagnitude)));

			NumberLocation += LocationOffset;
		}
	}

	if (UWorld* LocalWorld = GetWorld())
	{
		ALSRPGDamageNumActor* PopActor = LocalWorld->SpawnActor<ALSRPGDamageNumActor>(NumPopActorClass, NewRequest.WorldLocation, FRotator::ZeroRotator);

		PopActor->UpdateNum(FText::FromString(FString::FromInt(NewRequest.NumberToDisplay)));
		PopActor->UpdateNumColor(NewRequest.ColorToDisplay);

	}



}
