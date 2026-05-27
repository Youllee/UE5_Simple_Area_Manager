#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FAreaManagerEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	void OpenAreaManagerWindow();
	
private:
	void RegisterMenus();
	void HandleEditorInitialized(double Duration);
	void RegisterAreaVisualizer();
	void UnregisterAreaVisualizer();

private:
	TSharedPtr<class FUICommandList> CommandList;
	FDelegateHandle OnEditorInitializedHandle;
	bool bIsAreaVisualizerRegistered = false;

};
