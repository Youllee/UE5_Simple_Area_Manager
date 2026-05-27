#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "AreaManagerAreaActor.generated.h"

class AVolume;
class UAreaManagerSubsystem;
class UPrimitiveComponent;

// 에디터 비주얼라이저 등록을 위한 마커용 루트 컴포넌트입니다.
UCLASS()
class AREAMANAGER_API UAreaManagerAreaComponent : public USceneComponent
{
	GENERATED_BODY()
};

UCLASS(BlueprintType, Blueprintable)
class AREAMANAGER_API AAreaManagerAreaActor : public AActor
{
	GENERATED_BODY()

public:
	AAreaManagerAreaActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	UAreaManagerSubsystem* GetAreaManagerSubsystem() const;
	void InitializeAreaVolumes();
	void BindVolumeOverlapEvents();
	void UnbindVolumeOverlapEvents();
	void RegisterInitialOverlappingActors();

	UFUNCTION(Category = "Area Manager")
	void OnAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(Category = "Area Manager")
	void OnAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UFUNCTION(BlueprintPure, Category = "Area Manager")
	FName GetAreaName() const { return AreaName; }
	UFUNCTION(BlueprintPure, Category = "Area Manager")
	TArray<AVolume*> GetAreaVolumes() const;
	UFUNCTION(BlueprintNativeEvent, Category = "Area Manager")
	TArray<AVolume*> CollectAreaVolumes() const;
	virtual TArray<AVolume*> CollectAreaVolumes_Implementation() const;
	UFUNCTION(BlueprintPure, Category = "Area Manager")
	TArray<AAreaManagerAreaActor*> GetConnectedAreas() const;

#if WITH_EDITOR
	// 볼륨이 겹쳐 있는 Area를 Connected Area에 추가합니다. 기존 연결은 유지됩니다.
	UFUNCTION(CallInEditor, Category = "Area Manager")
	void CollectConnectedAreas();
#endif

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area Manager")
	FName AreaName = FName();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area Manager")
	TArray<TObjectPtr<AAreaManagerAreaActor>> ConnectedAreas;
	UPROPERTY(Transient)
	TArray<TObjectPtr<AVolume>> CachedAreaVolumes;

};
