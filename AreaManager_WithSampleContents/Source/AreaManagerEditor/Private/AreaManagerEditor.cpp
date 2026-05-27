#include "AreaManagerEditor.h"
#include "AreaManagerAreaActor.h"
#include "AreaManagerVisualizer.h"
#include "AreaManagerStyle.h"
#include "AreaManagerCommands.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "ToolMenus.h"

#include "Editor.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"

DEFINE_LOG_CATEGORY_STATIC(LogAreaManagerEditor, Log, All);

namespace AreaManagerEditor
{
	static constexpr const TCHAR* EditorUtilityWidgetPath = TEXT("/AreaManager/UI/Widget/EUW_AreaManager.EUW_AreaManager");
}

void FAreaManagerEditorModule::StartupModule()
{
	FAreaManagerStyle::Initialize();
	FAreaManagerStyle::ReloadTextures();

	FAreaManagerCommands::Register();
	CommandList = MakeShareable(new FUICommandList);
	CommandList->MapAction(
		FAreaManagerCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FAreaManagerEditorModule::OpenAreaManagerWindow),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FAreaManagerEditorModule::RegisterMenus));

	if (GUnrealEd != nullptr)
	{
		RegisterAreaVisualizer();
	}
	else
	{
		OnEditorInitializedHandle = FEditorDelegates::OnEditorInitialized.AddRaw(this, &FAreaManagerEditorModule::HandleEditorInitialized);
	}
}

void FAreaManagerEditorModule::ShutdownModule()
{
	if (OnEditorInitializedHandle.IsValid())
	{
		FEditorDelegates::OnEditorInitialized.Remove(OnEditorInitializedHandle);
		OnEditorInitializedHandle.Reset();
	}

	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	UnregisterAreaVisualizer();
	FAreaManagerCommands::Unregister();
	FAreaManagerStyle::Shutdown();
	CommandList.Reset();
}

void FAreaManagerEditorModule::OpenAreaManagerWindow()
{
	if (GEditor == nullptr)
	{
		return;
	}

	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
	if (EditorUtilitySubsystem == nullptr)
	{
		UE_LOG(LogAreaManagerEditor, Warning, TEXT("AreaManager 에디터 유틸리티 위젯을 열 수 없습니다. EditorUtilitySubsystem을 사용할 수 없습니다."));
		return;
	}

	UEditorUtilityWidgetBlueprint* WidgetBlueprint = LoadObject<UEditorUtilityWidgetBlueprint>(nullptr, AreaManagerEditor::EditorUtilityWidgetPath);
	if (WidgetBlueprint == nullptr)
	{
		UE_LOG(LogAreaManagerEditor, Warning, TEXT("AreaManager 에디터 유틸리티 위젯을 열 수 없습니다. '%s' 에셋을 찾지 못했습니다."), AreaManagerEditor::EditorUtilityWidgetPath);
		return;
	}

	EditorUtilitySubsystem->SpawnAndRegisterTab(WidgetBlueprint);
}

void FAreaManagerEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* WindowMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
	FToolMenuSection& WindowSection = WindowMenu->FindOrAddSection("WindowLayout");
	WindowSection.AddMenuEntryWithCommandList(FAreaManagerCommands::Get().OpenPluginWindow, CommandList);

	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& ToolbarSection = ToolbarMenu->FindOrAddSection("PluginTools");
	FToolMenuEntry& ToolbarEntry = ToolbarSection.AddEntry(FToolMenuEntry::InitToolBarButton(FAreaManagerCommands::Get().OpenPluginWindow));
	ToolbarEntry.SetCommandList(CommandList);
}

void FAreaManagerEditorModule::HandleEditorInitialized(double Duration)
{
	if (OnEditorInitializedHandle.IsValid())
	{
		FEditorDelegates::OnEditorInitialized.Remove(OnEditorInitializedHandle);
		OnEditorInitializedHandle.Reset();
	}

	RegisterAreaVisualizer();
}

void FAreaManagerEditorModule::RegisterAreaVisualizer()
{
	if (bIsAreaVisualizerRegistered == true || GUnrealEd == nullptr)
	{
		return;
	}
	bIsAreaVisualizerRegistered = true;

	TSharedPtr<FComponentVisualizer> AreaVisualizer = MakeShareable(new FAreaManagerVisualizer);
	GUnrealEd->RegisterComponentVisualizer(UAreaManagerAreaComponent::StaticClass()->GetFName(), AreaVisualizer);
	AreaVisualizer->OnRegister();
}

void FAreaManagerEditorModule::UnregisterAreaVisualizer()
{
	if (bIsAreaVisualizerRegistered == false || GUnrealEd == nullptr)
	{
		return;
	}
	bIsAreaVisualizerRegistered = false;

	GUnrealEd->UnregisterComponentVisualizer(UAreaManagerAreaComponent::StaticClass()->GetFName());
}

IMPLEMENT_MODULE(FAreaManagerEditorModule, AreaManagerEditor)
