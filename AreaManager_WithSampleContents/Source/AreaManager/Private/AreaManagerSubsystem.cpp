#include "AreaManagerSubsystem.h"
#include "AreaManagerAreaActor.h"
#include "AreaManagerMemberComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Volume.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogAreaManager, Log, All);

bool UAreaManagerSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return (WorldType == EWorldType::PIE) || (WorldType == EWorldType::Game);
}

bool UAreaManagerSubsystem::RegisterAreaActor(AAreaManagerAreaActor* AreaActor)
{
	if (IsValid(AreaActor) == false)
	{
		return false;
	}

	const FName AreaName = AreaActor->GetAreaName();
	if (AreaName.IsNone())
	{
		return false;
	}

	if (GetAreaData(AreaName) != nullptr)
	{
		UE_LOG(LogAreaManager, Warning, TEXT("Area '%s'는 이미 등록되어 있습니다."), *AreaName.ToString());
		return false;
	}

	AreaDatas.Add(AreaName, FAreaManagerAreaData(AreaActor));
	if (OnRegisteredArea.IsBound())
	{
		OnRegisteredArea.Broadcast(AreaName);
	}

	return true;
}

void UAreaManagerSubsystem::UnregisterAreaActor(AAreaManagerAreaActor* AreaActor)
{
	if (IsValid(AreaActor) == false)
	{
		return;
	}

	const FName AreaName = AreaActor->GetAreaName();
	if (AreaName.IsNone())
	{
		return;
	}

	FAreaManagerAreaData* AreaData = AreaDatas.Find(AreaName);
	if (AreaData == nullptr)
	{
		return;
	}

	TArray<TObjectPtr<AActor>> MemberActors = AreaData->Members;
	for (AActor* MemberActor : MemberActors)
	{
		UnregisterAreaMember(MemberActor);
	}

	AreaDatas.Remove(AreaName);
	if (OnUnregisteredArea.IsBound())
	{
		OnUnregisteredArea.Broadcast(AreaName);
	}
}

bool UAreaManagerSubsystem::RegisterAreaMember(AAreaManagerAreaActor* AreaActor, AActor* MemberActor)
{
	if (IsValid(AreaActor) == false || IsValid(MemberActor) == false)
	{
		return false;
	}

	if (AreaActor == MemberActor || AreaActor->GetAreaVolumes().Contains(MemberActor))
	{
		return false;
	}

	const FName NewAreaName = AreaActor->GetAreaName();
	if (NewAreaName.IsNone())
	{
		return false;
	}

	FAreaManagerAreaData* NewAreaData = AreaDatas.Find(NewAreaName);
	if (NewAreaData == nullptr || NewAreaData->IsValid() == false)
	{
		return false;
	}

	UAreaManagerMemberComponent* MemberComponent = MemberActor->FindComponentByClass<UAreaManagerMemberComponent>();
	if (IsValid(MemberComponent) == false)
	{
		return false;
	}

	MemberComponent->AddOverlappingArea(AreaActor);

	const FName OldAreaName = MemberComponent->GetAreaName();
	if (OldAreaName == NewAreaName)
	{
		return false;
	}

	FAreaManagerAreaData* OldAreaData = AreaDatas.Find(OldAreaName);
	if (OldAreaData != nullptr)
	{
		if (OldAreaData->Members.Contains(MemberActor))
		{
			OldAreaData->Members.Remove(MemberActor);
		}
	}

	if (NewAreaData->Members.Contains(MemberActor) == false)
	{
		NewAreaData->Members.Add(MemberActor);
		MemberComponent->SetAreaName(NewAreaData->Area->GetAreaName());
		BroadcastAreaChanged(NewAreaName, OldAreaName, MemberActor);
	}

	return true;
}

bool UAreaManagerSubsystem::RegisterAreaMemberByLocation(AActor* MemberActor)
{
	if (IsValid(MemberActor) == false)
	{
		return false;
	}

	UAreaManagerMemberComponent* MemberComponent = MemberActor->FindComponentByClass<UAreaManagerMemberComponent>();
	if (IsValid(MemberComponent) == false)
	{
		return false;
	}

	const FVector MemberLocation = MemberActor->GetActorLocation();

	if (MemberComponent->GetAreaName().IsNone() == false)
	{
		const FAreaManagerAreaData* CurrentAreaData = AreaDatas.Find(MemberComponent->GetAreaName());
		AAreaManagerAreaActor* CurrentArea = (CurrentAreaData != nullptr) ? CurrentAreaData->Area.Get() : nullptr;
		if (IsValid(CurrentArea))
		{
			for (AVolume* Volume : CurrentArea->GetAreaVolumes())
			{
				if (IsValid(Volume) && Volume->EncompassesPoint(MemberLocation))
				{
					MemberComponent->AddOverlappingArea(CurrentArea);
					return true;
				}
			}
		}
	}

	for (const TPair<FName, FAreaManagerAreaData>& AreaDataPair : AreaDatas)
	{
		if (AAreaManagerAreaActor* Area = AreaDataPair.Value.Area.Get())
		{
			for (AVolume* Volume : Area->GetAreaVolumes())
			{
				if (IsValid(Volume) && Volume->EncompassesPoint(MemberLocation))
				{
					return RegisterAreaMember(Area, MemberActor);
				}
			}
		}
	}

	return false;
}

void UAreaManagerSubsystem::UnregisterAreaMember(AActor* MemberActor)
{
	if (IsValid(MemberActor))
	{
		UnregisterAreaMember(MemberActor, MemberActor->FindComponentByClass<UAreaManagerMemberComponent>());
	}
}

void UAreaManagerSubsystem::UnregisterAreaMember(UAreaManagerMemberComponent* MemberComponent)
{
	if (IsValid(MemberComponent))
	{
		UnregisterAreaMember(MemberComponent->GetOwner(), MemberComponent);
	}
}

void UAreaManagerSubsystem::UnregisterAreaMember(AActor* MemberActor, UAreaManagerMemberComponent* MemberComponent)
{
	if (IsValid(MemberActor) == false || IsValid(MemberComponent) == false)
	{
		return;
	}

	FName AreaName = MemberComponent->GetAreaName();
	if (AreaName.IsNone())
	{
		return;
	}

	FAreaManagerAreaData* AreaData = GetAreaData(AreaName);
	if (AreaData == nullptr || AreaData->Members.Contains(MemberActor) == false)
	{
		return;
	}

	AreaData->Members.Remove(MemberActor);
	MemberComponent->SetAreaName(FName());

	BroadcastAreaChanged(FName(), AreaName, MemberActor);
}

void UAreaManagerSubsystem::BroadcastAreaChanged(FName NewAreaName, FName OldAreaName, AActor* MemberActor)
{
	if (IsValid(MemberActor) == false)
	{
		return;
	}

	const UWorld* World = GetWorld();
	const APawn* LocalPlayerPawn = IsValid(World) ? UGameplayStatics::GetPlayerPawn(World, 0) : nullptr;
	if (MemberActor == LocalPlayerPawn)
	{
		if (OnPlayerAreaChanged.IsBound())
		{
			OnPlayerAreaChanged.Broadcast(NewAreaName, OldAreaName, MemberActor);
		}
	}
	else
	{
		if (OnMemberAreaChanged.IsBound())
		{
			OnMemberAreaChanged.Broadcast(NewAreaName, OldAreaName, MemberActor);
		}
	}
}

FAreaManagerAreaData* UAreaManagerSubsystem::GetAreaData(FName AreaName)
{
	return (AreaName.IsNone() == false) ? AreaDatas.Find(AreaName) : nullptr;
}

bool UAreaManagerSubsystem::FindAreaData(FName AreaName, FAreaManagerAreaData& AreaData) const
{
	const FAreaManagerAreaData* FoundAreaData = (AreaName.IsNone() == false) ? AreaDatas.Find(AreaName) : nullptr;
	if (FoundAreaData == nullptr)
	{
		return false;
	}

	AreaData = *FoundAreaData;
	return true;
}

AAreaManagerAreaActor* UAreaManagerSubsystem::GetCurrentPlayerArea(int32 PlayerIndex) const
{
	const UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		return nullptr;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, PlayerIndex);
	if (IsValid(PlayerPawn) == false)
	{
		return nullptr;
	}

	const UAreaManagerMemberComponent* MemberComponent = PlayerPawn->FindComponentByClass<UAreaManagerMemberComponent>();
	if (IsValid(MemberComponent) == false)
	{
		return nullptr;
	}

	const FAreaManagerAreaData* AreaData = AreaDatas.Find(MemberComponent->GetAreaName());
	return (AreaData != nullptr) ? AreaData->Area.Get() : nullptr;
}

TArray<AAreaManagerAreaActor*> UAreaManagerSubsystem::GetAreaActors(EAreaManagerRangeType InRange) const
{
	AAreaManagerAreaActor* CurrentArea = nullptr;
	TArray<AAreaManagerAreaActor*> Result;
	switch (InRange)
	{
	case EAreaManagerRangeType::All:
		for (const TPair<FName, FAreaManagerAreaData>& AreaData : AreaDatas)
		{
			AAreaManagerAreaActor* Area = AreaData.Value.Area.Get();
			if (IsValid(Area))
			{
				Result.AddUnique(Area);
			}
		}
		break;
	case EAreaManagerRangeType::Connected:
		CurrentArea = GetCurrentPlayerArea();
		if (IsValid(CurrentArea))
		{
			Result.AddUnique(CurrentArea);
			for (AAreaManagerAreaActor* ConnectedArea : CurrentArea->GetConnectedAreas())
			{
				if (IsValid(ConnectedArea))
				{
					Result.AddUnique(ConnectedArea);
				}
			}
		}
		break;
	case EAreaManagerRangeType::Current:
		CurrentArea = GetCurrentPlayerArea();
		if (IsValid(CurrentArea))
		{
			Result.AddUnique(CurrentArea);
		}
		break;
	}

	return Result;
}

TArray<AActor*> UAreaManagerSubsystem::GetAreaMembers(EAreaManagerRangeType InRange) const
{
	TArray<AActor*> Result;
	TArray<AAreaManagerAreaActor*> AreaArray = GetAreaActors(InRange);
	for (AAreaManagerAreaActor* Area : AreaArray)
	{
		if (IsValid(Area))
		{
			const FAreaManagerAreaData* AreaData = AreaDatas.Find(Area->GetAreaName());
			if (AreaData != nullptr)
			{
				for (AActor* Member : AreaData->Members)
				{
					if (IsValid(Member))
					{
						Result.AddUnique(Member);
					}
				}
			}
		}
	}

	return Result;
}
