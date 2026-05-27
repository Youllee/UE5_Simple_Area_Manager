#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AreaManagerType.h"
#include "AreaManagerMemberComponent.generated.h"

class AAreaManagerAreaActor;
class UAreaManagerSubsystem;

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class AREAMANAGER_API UAreaManagerMemberComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAreaManagerMemberComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	bool SetAreaName(FName NewAreaName);
	void AddOverlappingArea(AAreaManagerAreaActor* AreaActor);
	void RemoveOverlappingArea(AAreaManagerAreaActor* AreaActor);
	bool ResolveCurrentArea();

	UFUNCTION(BlueprintPure, Category = "Area Manager")
	FName GetAreaName() const { return CachedAreaName; }
	UFUNCTION(BlueprintPure, Category = "Area Manager")
	const FAreaManagerIconData& GetIconData() const { return IconData; }

protected:
	UAreaManagerSubsystem* GetAreaManagerSubsystem() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area Manager")
	FAreaManagerIconData IconData;
	UPROPERTY(Transient, VisibleAnywhere, Category = "Area Manager", meta = (DisplayName = "Area Name"))
	FName CachedAreaName = FName();
	UPROPERTY(Transient)
	TArray<TObjectPtr<AAreaManagerAreaActor>> OverlappingAreas;

};
