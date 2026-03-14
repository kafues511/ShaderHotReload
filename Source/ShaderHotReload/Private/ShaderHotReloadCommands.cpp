// Copyright © 2025-2026 kafues511 All Rights Reserved.

#include "ShaderHotReloadCommands.h"

#define LOCTEXT_NAMESPACE "FShaderHotReloadModule"

void FShaderHotReloadCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "ShaderHotReload", "Execute ShaderHotReload action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
