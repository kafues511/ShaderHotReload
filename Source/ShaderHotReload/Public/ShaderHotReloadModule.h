// Copyright © 2025-2026 kafues511 All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Engine/EngineTypes.h"

DECLARE_LOG_CATEGORY_EXTERN(LogShaderHotReload, Log, All);

class FToolBarBuilder;
class FMenuBuilder;

class FShaderHotReloadModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** This function will be bound to Command. */
	void PluginButtonClicked();
	
private:
	void RegisterMenus();
	void CheckShaderCompilationState();
	void RestoreRendering();

private:
	TSharedPtr<class FUICommandList> PluginCommands;

	FTimerHandle CheckTimerHandle;
	bool bIsHotReloading = false;
	bool bHasStartedCompiling = false;
	double HotReloadStartTime = 0.0;
};
