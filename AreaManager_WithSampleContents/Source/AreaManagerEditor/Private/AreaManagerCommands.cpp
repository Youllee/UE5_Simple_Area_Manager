#include "AreaManagerCommands.h"
#include "AreaManagerStyle.h"

#define LOCTEXT_NAMESPACE "FAreaManagerModule"

FAreaManagerCommands::FAreaManagerCommands()
	: TCommands<FAreaManagerCommands>(
		TEXT("AreaManager"),
		NSLOCTEXT("Contexts", "AreaManager", "AreaManager Plugin"),
		NAME_None,
		FAreaManagerStyle::GetStyleSetName())
{
}

void FAreaManagerCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "AreaManager", "Bring up 'Simple Area Manager' window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
