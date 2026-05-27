#include "AreaManagerMemberComponent.h"
#include "AreaManagerAreaActor.h"
#include "AreaManagerSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/Volume.h"

namespace
{
	bool DoesAreaContainLocation(AAreaManagerAreaActor* AreaActor, const FVector& Location)
	{
		if (IsValid(AreaActor))
		{
			for (AVolume* Volume : AreaActor->GetAreaVolumes())
			{
				if (IsValid(Volume) && Volume->EncompassesPoint(Location))
				{
					return true;
				}
			}
		}

		return false;
	}
}

UAreaManagerMemberComponent::UAreaManagerMemberComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAreaManagerMemberComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UAreaManagerSubsystem* AreaManagerSubsystem = GetAreaManagerSubsystem())
	{
		AreaManagerSubsystem->RegisterAreaMemberByLocation(GetOwner());
	}
}

void UAreaManagerMemberComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UAreaManagerSubsystem* AreaManagerSubsystem = GetAreaManagerSubsystem())
	{
		AreaManagerSubsystem->UnregisterAreaMember(this);
	}

	Super::EndPlay(EndPlayReason);
}

bool UAreaManagerMemberComponent::SetAreaName(FName NewAreaName)
{
	if (CachedAreaName != NewAreaName)
	{
		CachedAreaName = NewAreaName;
		return true;
	}

	return false;	
}

void UAreaManagerMemberComponent::AddOverlappingArea(AAreaManagerAreaActor* AreaActor)
{
	if (IsValid(AreaActor))
	{
		OverlappingAreas.AddUnique(AreaActor);
	}
}

void UAreaManagerMemberComponent::RemoveOverlappingArea(AAreaManagerAreaActor* AreaActor)
{
	if (IsValid(AreaActor))
	{
		OverlappingAreas.Remove(AreaActor);
	}
}

UAreaManagerSubsystem* UAreaManagerMemberComponent::GetAreaManagerSubsystem() const
{
	UWorld* World = GetWorld();
	return IsValid(World) ? World->GetSubsystem<UAreaManagerSubsystem>() : nullptr;
}

bool UAreaManagerMemberComponent::ResolveCurrentArea()
{
	AActor* OwnerActor = GetOwner();
	UAreaManagerSubsystem* AreaManagerSubsystem = GetAreaManagerSubsystem();
	if (IsValid(OwnerActor) == false || IsValid(AreaManagerSubsystem) == false)
	{
		return false;
	}

	const FVector OwnerLocation = OwnerActor->GetActorLocation();

	OverlappingAreas.RemoveAll([](const TObjectPtr<AAreaManagerAreaActor>& Area)
	{
		return IsValid(Area) == false;
	});

	// 현재 등록된 Area가 아직 실제 위치를 포함하고 있다면, 경계에서 불필요하게 Area가 흔들리지 않도록 유지합니다.
	for (AAreaManagerAreaActor* Area : OverlappingAreas)
	{
		if (Area->GetAreaName() == CachedAreaName)
		{
			if (DoesAreaContainLocation(Area, OwnerLocation))
			{
				return false;
			}
		}
	}

	for (AAreaManagerAreaActor* Area : OverlappingAreas)
	{
		if (DoesAreaContainLocation(Area, OwnerLocation))
		{
			return AreaManagerSubsystem->RegisterAreaMember(Area, OwnerActor);
		}
	}

	if (AreaManagerSubsystem->RegisterAreaMemberByLocation(OwnerActor))
	{
		return true;
	}

	AreaManagerSubsystem->UnregisterAreaMember(OwnerActor, this);
	return true;
}
