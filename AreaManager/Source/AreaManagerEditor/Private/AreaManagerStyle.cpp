#include "AreaManagerStyle.h"

#include "Framework/Application/SlateApplication.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateStyleRegistry.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FAreaManagerStyle::StyleInstance = nullptr;

void FAreaManagerStyle::Initialize()
{
	if (StyleInstance.IsValid() == false)
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FAreaManagerStyle::Shutdown()
{
	if (StyleInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
		ensure(StyleInstance.IsUnique());
		StyleInstance.Reset();
	}
}

FName FAreaManagerStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("AreaManagerStyle"));
	return StyleSetName;
}

const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef<FSlateStyleSet> FAreaManagerStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("AreaManagerStyle"));

	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("AreaManager"));
	const FString ContentRoot = Plugin.IsValid()
		? Plugin->GetBaseDir() / TEXT("Resources")
		: FPaths::ProjectPluginsDir() / TEXT("AreaManager/Resources");

	Style->SetContentRoot(ContentRoot);
	Style->Set("AreaManager.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));
	return Style;
}

void FAreaManagerStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FAreaManagerStyle::Get()
{
	return *StyleInstance;
}
