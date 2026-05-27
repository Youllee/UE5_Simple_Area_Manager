#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "AreaManagerEditorSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAreaManagerPIEEvent, bool, bIsSimulating);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAreaManagerMapOpenedEvent, const FString&, Filename, bool, bAsTemplate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAreaManagerPreLoadMapEvent, UWorld*, World, const FString&, MapName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAreaManagerPostLoadMapEvent, UWorld*, LoadedWorld);

UCLASS()
class AREAMANAGEREDITOR_API UAreaManagerEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	void BindDelegates();
	void UnbindDelegates();

public:
	UFUNCTION(BlueprintPure, Category = "Area Manager|Editor")
	UWorld* GetEditorWorld() const;
	UFUNCTION(BlueprintPure, Category = "Area Manager|Editor")
	UWorld* GetPIEWorld() const;

public:
	UPROPERTY(BlueprintAssignable, Category = "Area Manager|Editor")
	FAreaManagerPIEEvent OnStartPIE;
	UPROPERTY(BlueprintAssignable, Category = "Area Manager|Editor", meta = (DisplayName = "On Post PIE Started"))
	FAreaManagerPIEEvent OnPostPIEStarted;
	UPROPERTY(BlueprintAssignable, Category = "Area Manager|Editor", meta = (DisplayName = "On Pre PIE Ended"))
	FAreaManagerPIEEvent OnPrePIEEnded;
	UPROPERTY(BlueprintAssignable, Category = "Area Manager|Editor")
	FAreaManagerPIEEvent OnEndPIE;
	UPROPERTY(BlueprintAssignable, Category = "Area Manager|Editor")
	FAreaManagerMapOpenedEvent OnEditorMapOpened;
	UPROPERTY(BlueprintAssignable, Category = "Area Manager|Editor")
	FAreaManagerPreLoadMapEvent OnPreLoadMap;
	UPROPERTY(BlueprintAssignable, Category = "Area Manager|Editor")
	FAreaManagerPostLoadMapEvent OnPostLoadMap;

private:
	FDelegateHandle StartPIEHandle;
	FDelegateHandle PostPIEStartedHandle;
	FDelegateHandle PrePIEEndedHandle;
	FDelegateHandle EndPIEHandle;
	FDelegateHandle MapOpenedHandle;
	FDelegateHandle PreLoadMapHandle;
	FDelegateHandle PostLoadMapHandle;

};
