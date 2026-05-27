#include "AreaManagerFunctionLibrary.h"
#include "Components/BrushComponent.h"
#include "GameFramework/Volume.h"

namespace
{
	bool GetAreaVolumeWorldTransform(AVolume* AreaVolume, FVector& WorldCenter, FVector2D& WorldSize, float& Yaw)
	{
		WorldCenter = FVector::ZeroVector;
		WorldSize = FVector2D::ZeroVector;
		Yaw = 0.0f;

		if (IsValid(AreaVolume) == false)
		{
			return false;
		}

		const UBrushComponent* BrushComponent = AreaVolume->GetBrushComponent();
		if (IsValid(BrushComponent) == false)
		{
			return false;
		}

		const FBox LocalBox = BrushComponent->CalcBounds(FTransform::Identity).GetBox();
		if (LocalBox.IsValid == false)
		{
			return false;
		}

		const FVector LocalBoundsSize = LocalBox.GetSize();
		const FVector ComponentScale = BrushComponent->GetComponentScale();
		WorldCenter = BrushComponent->GetComponentTransform().TransformPosition(LocalBox.GetCenter());
		WorldSize = FVector2D(
			LocalBoundsSize.X * FMath::Abs(ComponentScale.X),
			LocalBoundsSize.Y * FMath::Abs(ComponentScale.Y));
		Yaw = BrushComponent->GetComponentRotation().Yaw;

		return true;
	}
}

bool UAreaManagerFunctionLibrary::GetAreaVolumeMinimapTransform(AActor* AreaVolumeActor, FVector MinimapCenter, FVector2D& RelativeCenter, FVector2D& WorldSize, float& Yaw)
{
	RelativeCenter = FVector2D::ZeroVector;
	WorldSize = FVector2D::ZeroVector;
	Yaw = 0.0f;

	FVector VolumeWorldCenter = FVector::ZeroVector;
	if (GetAreaVolumeWorldTransform(Cast<AVolume>(AreaVolumeActor), VolumeWorldCenter, WorldSize, Yaw) == false)
	{
		return false;
	}

	RelativeCenter = FVector2D(VolumeWorldCenter - MinimapCenter);

	return true;
}
