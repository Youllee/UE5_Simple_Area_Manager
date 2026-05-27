#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AreaManagerType.h"
#include "AreaManagerSubsystem.generated.h"

class AAreaManagerAreaActor;
class UAreaManagerMemberComponent;

UCLASS(meta = (DisplayName = "Area Manager Subsystem"))
class AREAMANAGER_API UAreaManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Area Manager")
	FAreaManagerDelegate_NameParam OnRegisteredArea;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Area Manager")
	FAreaManagerDelegate_NameParam OnUnregisteredArea;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Area Manager")
	FAreaManagerDelegate_AreaChanged OnPlayerAreaChanged;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Area Manager")
	FAreaManagerDelegate_AreaChanged OnMemberAreaChanged;

	bool RegisterAreaActor(AAreaManagerAreaActor* AreaActor);
	void UnregisterAreaActor(AAreaManagerAreaActor* AreaActor);

	bool RegisterAreaMember(AAreaManagerAreaActor* AreaActor, AActor* MemberActor);
	bool RegisterAreaMemberByLocation(AActor* MemberActor);
	void UnregisterAreaMember(AActor* MemberActor);
	void UnregisterAreaMember(UAreaManagerMemberComponent* MemberComponent);
	void UnregisterAreaMember(AActor* MemberActor, UAreaManagerMemberComponent* MemberComponent);

protected:
	void BroadcastAreaChanged(FName NewAreaName, FName OldAreaName, AActor* MemberActor);
	FAreaManagerAreaData* GetAreaData(FName AreaName);

public:
	UFUNCTION(BlueprintPure, Category = "Area Manager")
	bool FindAreaData(FName AreaName, FAreaManagerAreaData& AreaData) const;
	UFUNCTION(BlueprintPure, Category = "Area Manager")
	AAreaManagerAreaActor* GetCurrentPlayerArea(int32 PlayerIndex = 0) const;
	UFUNCTION(BlueprintPure, Category = "Area Manager")
	TArray<AAreaManagerAreaActor*> GetAreaActors(EAreaManagerRangeType InRange) const;
	UFUNCTION(BlueprintPure, Category = "Area Manager")
	TArray<AActor*> GetAreaMembers(EAreaManagerRangeType InRange) const;

protected:
	UPROPERTY(Transient)
	TMap<FName, FAreaManagerAreaData> AreaDatas;

};
