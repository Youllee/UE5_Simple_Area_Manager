#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AreaManagerFunctionLibrary.generated.h"

class AActor;

UCLASS()
class AREAMANAGER_API UAreaManagerFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Area Volume을 미니맵에 배치하기 위한 상대 위치, 크기, 회전값을 반환합니다.
	UFUNCTION(BlueprintPure, Category = "Area Manager", meta = (DisplayName = "Get Area Volume Transform (for Minimap)"))
	static bool GetAreaVolumeMinimapTransform(AActor* AreaVolumeActor, FVector MinimapCenter, FVector2D& RelativeCenter, FVector2D& WorldSize, float& Yaw);

};
