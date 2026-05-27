#pragma once

#include "Framework/Commands/Commands.h"

class FAreaManagerCommands : public TCommands<FAreaManagerCommands>
{
public:
	FAreaManagerCommands();

	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenPluginWindow;
};
