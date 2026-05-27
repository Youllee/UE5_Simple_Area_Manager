#pragma once

#include "CoreMinimal.h"
#include "AreaManagerType.generated.h"

class UTexture2D;
class UUserWidget;
class AActor;
class AAreaManagerAreaActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAreaManagerDelegate_NameParam, FName, InName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAreaManagerDelegate_AreaChanged, FName, InNewAreaName, FName, InOldAreaName, AActor*, InMemberActor);

// Area 표시 범위 (for Minimap)
UENUM(BlueprintType)
enum class EAreaManagerRangeType : uint8
{
	None,
	All,
	Connected,
	Current,
};

// Area 중심 기준 (for Minimap)
UENUM(BlueprintType)
enum class EAreaManagerCenterType : uint8
{
	None,
	World,
	Area,
};

USTRUCT(BlueprintType)
struct FAreaManagerAreaData
{
	GENERATED_BODY()

public:
	FAreaManagerAreaData() {}
	FAreaManagerAreaData(AAreaManagerAreaActor* InArea) : Area(InArea) {}

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Area Manager")
	TObjectPtr<AAreaManagerAreaActor> Area = nullptr;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Area Manager")
	TArray<TObjectPtr<AActor>> Members;

public:
	bool IsValid() const;

};

USTRUCT(BlueprintType)
struct FAreaManagerIconData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area Manager")
	TSoftClassPtr<UUserWidget> IconWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area Manager")
	TSoftObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area Manager", meta = (DisplayName = "Angle Offset (Degree)"))
	float AngleOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area Manager")
	FLinearColor IconColor = FLinearColor::White;

};
