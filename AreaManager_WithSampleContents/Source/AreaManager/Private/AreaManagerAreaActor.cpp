#include "AreaManagerAreaActor.h"
#include "AreaManagerMemberComponent.h"
#include "AreaManagerSubsystem.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Volume.h"
#include "Engine/World.h"

#if WITH_EDITOR
#include "EngineUtils.h"
#endif

namespace
{
	UPrimitiveComponent* GetVolumeRootPrimitive(AVolume* Volume)
	{
		return IsValid(Volume) ? Cast<UPrimitiveComponent>(Volume->GetRootComponent()) : nullptr;
	}
}

AAreaManagerAreaActor::AAreaManagerAreaActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* AreaRootComponent = CreateDefaultSubobject<UAreaManagerAreaComponent>(TEXT("SceneRoot"));
	AreaRootComponent->Mobility = EComponentMobility::Static;
	SetRootComponent(AreaRootComponent);

#if WITH_EDITOR
	bIsSpatiallyLoaded = false;
#endif
}

void AAreaManagerAreaActor::BeginPlay()
{
	Super::BeginPlay();

	InitializeAreaVolumes();

	if (UAreaManagerSubsystem* AreaManagerSubsystem = GetAreaManagerSubsystem())
	{
		if (AreaManagerSubsystem->RegisterAreaActor(this))
		{
			BindVolumeOverlapEvents();
			RegisterInitialOverlappingActors();
		}
	}
}

void AAreaManagerAreaActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UAreaManagerSubsystem* AreaManagerSubsystem = GetAreaManagerSubsystem())
	{
		AreaManagerSubsystem->UnregisterAreaActor(this);
	}

	UnbindVolumeOverlapEvents();
	CachedAreaVolumes.Reset();

	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void AAreaManagerAreaActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	bIsSpatiallyLoaded = false;
}
#endif

UAreaManagerSubsystem* AAreaManagerAreaActor::GetAreaManagerSubsystem() const
{
	UWorld* World = GetWorld();
	return IsValid(World) ? World->GetSubsystem<UAreaManagerSubsystem>() : nullptr;
}

void AAreaManagerAreaActor::InitializeAreaVolumes()
{
	CachedAreaVolumes.Reset();

	const TArray<AVolume*> CollectedVolumes = CollectAreaVolumes();
	for (AVolume* Volume : CollectedVolumes)
	{
		if (IsValid(Volume))
		{
			CachedAreaVolumes.AddUnique(Volume);
		}
	}
}

void AAreaManagerAreaActor::BindVolumeOverlapEvents()
{
	for (AVolume* Volume : CachedAreaVolumes)
	{
		UPrimitiveComponent* VolumeComponent = GetVolumeRootPrimitive(Volume);
		if (IsValid(VolumeComponent))
		{
			VolumeComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AAreaManagerAreaActor::OnAreaBeginOverlap);
			VolumeComponent->OnComponentEndOverlap.RemoveDynamic(this, &AAreaManagerAreaActor::OnAreaEndOverlap);

			VolumeComponent->OnComponentBeginOverlap.AddDynamic(this, &AAreaManagerAreaActor::OnAreaBeginOverlap);
			VolumeComponent->OnComponentEndOverlap.AddDynamic(this, &AAreaManagerAreaActor::OnAreaEndOverlap);
		}
	}
}

void AAreaManagerAreaActor::UnbindVolumeOverlapEvents()
{
	for (AVolume* Volume : CachedAreaVolumes)
	{
		UPrimitiveComponent* VolumeComponent = GetVolumeRootPrimitive(Volume);
		if (IsValid(VolumeComponent))
		{
			VolumeComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AAreaManagerAreaActor::OnAreaBeginOverlap);
			VolumeComponent->OnComponentEndOverlap.RemoveDynamic(this, &AAreaManagerAreaActor::OnAreaEndOverlap);
		}
	}
}

void AAreaManagerAreaActor::RegisterInitialOverlappingActors()
{
	UAreaManagerSubsystem* AreaManagerSubsystem = GetAreaManagerSubsystem();
	if (IsValid(AreaManagerSubsystem) == false)
	{
		return;
	}

	for (AVolume* Volume : CachedAreaVolumes)
	{
		UPrimitiveComponent* VolumeComponent = GetVolumeRootPrimitive(Volume);
		if (IsValid(VolumeComponent) == false)
		{
			continue;
		}

		VolumeComponent->UpdateOverlaps();

		TArray<AActor*> OverlappingActors;
		VolumeComponent->GetOverlappingActors(OverlappingActors);
		for (AActor* OverlappingActor : OverlappingActors)
		{
			AreaManagerSubsystem->RegisterAreaMember(this, OverlappingActor);
		}
	}
}

void AAreaManagerAreaActor::OnAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (UAreaManagerSubsystem* AreaManagerSubsystem = GetAreaManagerSubsystem())
	{
		AreaManagerSubsystem->RegisterAreaMember(this, OtherActor);
	}
}

void AAreaManagerAreaActor::OnAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UAreaManagerMemberComponent* MemberComponent = IsValid(OtherActor) ? OtherActor->FindComponentByClass<UAreaManagerMemberComponent>() : nullptr;
	if (IsValid(MemberComponent))
	{
		MemberComponent->RemoveOverlappingArea(this);
		MemberComponent->ResolveCurrentArea();
	}
}

TArray<AVolume*> AAreaManagerAreaActor::GetAreaVolumes() const
{
	TArray<AVolume*> AreaVolumes;

#if WITH_EDITOR
	UWorld* World = GetWorld();
	if (IsValid(World) && World->WorldType == EWorldType::Editor)
	{
		for (AVolume* Volume : CollectAreaVolumes())
		{
			if (IsValid(Volume))
			{
				AreaVolumes.AddUnique(Volume);
			}
		}

		return AreaVolumes;
	}
#endif

	for (AVolume* Volume : CachedAreaVolumes)
	{
		if (IsValid(Volume))
		{
			AreaVolumes.AddUnique(Volume);
		}
	}

	return AreaVolumes;
}

TArray<AAreaManagerAreaActor*> AAreaManagerAreaActor::GetConnectedAreas() const
{
	TArray<AAreaManagerAreaActor*> Result;
	for (AAreaManagerAreaActor* Area : ConnectedAreas)
	{
		if (IsValid(Area))
		{
			Result.AddUnique(Area);
		}
	}

	return Result;
}

TArray<AVolume*> AAreaManagerAreaActor::CollectAreaVolumes_Implementation() const
{
	TArray<AVolume*> AttachedVolumes;

	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	for (AActor* AttachedActor : AttachedActors)
	{
		if (AVolume* Volume = Cast<AVolume>(AttachedActor))
		{
			AttachedVolumes.AddUnique(Volume);
		}
	}

	return AttachedVolumes;
}

#if WITH_EDITOR
void AAreaManagerAreaActor::CollectConnectedAreas()
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		return;
	}

	const TArray<AVolume*> AreaVolumes = CollectAreaVolumes();
	TArray<AAreaManagerAreaActor*> FoundConnectedAreas;

	for (TActorIterator<AAreaManagerAreaActor> It(World); It; ++It)
	{
		AAreaManagerAreaActor* OtherArea = *It;
		if (IsValid(OtherArea) == false || OtherArea == this)
		{
			continue;
		}

		const TArray<AVolume*> OtherVolumes = OtherArea->CollectAreaVolumes();
		bool bIsConnected = false;

		for (AVolume* AreaVolume : AreaVolumes)
		{
			UPrimitiveComponent* AreaVolumeComponent = GetVolumeRootPrimitive(AreaVolume);
			if (IsValid(AreaVolumeComponent) == false)
			{
				continue;
			}

			const FBox AreaBounds = AreaVolumeComponent->Bounds.GetBox();
			for (AVolume* OtherVolume : OtherVolumes)
			{
				UPrimitiveComponent* OtherVolumeComponent = GetVolumeRootPrimitive(OtherVolume);
				if (IsValid(OtherVolumeComponent) == false)
				{
					continue;
				}

				if (AreaBounds.Intersect(OtherVolumeComponent->Bounds.GetBox()))
				{
					bIsConnected = true;
					break;
				}
			}

			if (bIsConnected)
			{
				break;
			}
		}

		if (bIsConnected)
		{
			FoundConnectedAreas.AddUnique(OtherArea);
		}
	}

	Modify();
	for (AAreaManagerAreaActor* FoundArea : FoundConnectedAreas)
	{
		ConnectedAreas.AddUnique(FoundArea);
	}

	PostEditChange();
	MarkPackageDirty();
}
#endif
