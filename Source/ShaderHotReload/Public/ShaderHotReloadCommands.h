// Copyright © 2025-2026 kafues511 All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "ShaderHotReloadStyle.h"

class FShaderHotReloadCommands : public TCommands<FShaderHotReloadCommands>
{
public:

	FShaderHotReloadCommands()
		: TCommands<FShaderHotReloadCommands>(TEXT("ShaderHotReload"), NSLOCTEXT("Contexts", "ShaderHotReload", "ShaderHotReload Plugin"), NAME_None, FShaderHotReloadStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
