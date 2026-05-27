#include "AreaManagerEditorSubsystem.h"
#include "Editor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

void UAreaManagerEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	BindDelegates();
}

void UAreaManagerEditorSubsystem::Deinitialize()
{
	UnbindDelegates();

	Super::Deinitialize();
}

void UAreaManagerEditorSubsystem::BindDelegates()
{
	TWeakObjectPtr<UAreaManagerEditorSubsystem> WeakThis(this);

	StartPIEHandle = FEditorDelegates::StartPIE.AddLambda([WeakThis](bool bIsSimulating)
	{
		if (UAreaManagerEditorSubsystem* Subsystem = WeakThis.Get())
		{
			Subsystem->OnStartPIE.Broadcast(bIsSimulating);
		}
	});

	PostPIEStartedHandle = FEditorDelegates::PostPIEStarted.AddLambda([WeakThis](bool bIsSimulating)
	{
		if (UAreaManagerEditorSubsystem* Subsystem = WeakThis.Get())
		{
			Subsystem->OnPostPIEStarted.Broadcast(bIsSimulating);
		}
	});

	PrePIEEndedHandle = FEditorDelegates::PrePIEEnded.AddLambda([WeakThis](bool bIsSimulating)
	{
		if (UAreaManagerEditorSubsystem* Subsystem = WeakThis.Get())
		{
			Subsystem->OnPrePIEEnded.Broadcast(bIsSimulating);
		}
	});

	EndPIEHandle = FEditorDelegates::EndPIE.AddLambda([WeakThis](bool bIsSimulating)
	{
		if (UAreaManagerEditorSubsystem* Subsystem = WeakThis.Get())
		{
			Subsystem->OnEndPIE.Broadcast(bIsSimulating);
		}
	});

	MapOpenedHandle = FEditorDelegates::OnMapOpened.AddLambda([WeakThis](const FString& Filename, bool bAsTemplate)
	{
		if (UAreaManagerEditorSubsystem* Subsystem = WeakThis.Get())
		{
			Subsystem->OnEditorMapOpened.Broadcast(Filename, bAsTemplate);
		}
	});

	PreLoadMapHandle = FCoreUObjectDelegates::PreLoadMapWithContext.AddLambda([WeakThis](const FWorldContext& WorldContext, const FString& MapName)
	{
		if (UAreaManagerEditorSubsystem* Subsystem = WeakThis.Get())
		{
			Subsystem->OnPreLoadMap.Broadcast(WorldContext.World(), MapName);
		}
	});

	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddLambda([WeakThis](UWorld* LoadedWorld)
	{
		if (UAreaManagerEditorSubsystem* Subsystem = WeakThis.Get())
		{
			Subsystem->OnPostLoadMap.Broadcast(LoadedWorld);
		}
	});
}

void UAreaManagerEditorSubsystem::UnbindDelegates()
{
	if (StartPIEHandle.IsValid())
	{
		FEditorDelegates::StartPIE.Remove(StartPIEHandle);
		StartPIEHandle.Reset();
	}

	if (PostPIEStartedHandle.IsValid())
	{
		FEditorDelegates::PostPIEStarted.Remove(PostPIEStartedHandle);
		PostPIEStartedHandle.Reset();
	}

	if (PrePIEEndedHandle.IsValid())
	{
		FEditorDelegates::PrePIEEnded.Remove(PrePIEEndedHandle);
		PrePIEEndedHandle.Reset();
	}

	if (EndPIEHandle.IsValid())
	{
		FEditorDelegates::EndPIE.Remove(EndPIEHandle);
		EndPIEHandle.Reset();
	}

	if (MapOpenedHandle.IsValid())
	{
		FEditorDelegates::OnMapOpened.Remove(MapOpenedHandle);
		MapOpenedHandle.Reset();
	}

	if (PreLoadMapHandle.IsValid())
	{
		FCoreUObjectDelegates::PreLoadMapWithContext.Remove(PreLoadMapHandle);
		PreLoadMapHandle.Reset();
	}

	if (PostLoadMapHandle.IsValid())
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);
		PostLoadMapHandle.Reset();
	}
}

UWorld* UAreaManagerEditorSubsystem::GetEditorWorld() const
{
	return GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
}

UWorld* UAreaManagerEditorSubsystem::GetPIEWorld() const
{
	if (GEngine == nullptr)
	{
		return nullptr;
	}

	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		if (WorldContext.WorldType == EWorldType::PIE && WorldContext.RunAsDedicated == false)
		{
			return WorldContext.World();
		}
	}

	return nullptr;
}
