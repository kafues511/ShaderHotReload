// Copyright © 2025-2026 kafues511 All Rights Reserved.

#include "ShaderHotReloadStyle.h"
#include "ShaderHotReloadModule.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FShaderHotReloadStyle::StyleInstance = nullptr;

void FShaderHotReloadStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FShaderHotReloadStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FShaderHotReloadStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ShaderHotReloadStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FShaderHotReloadStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("ShaderHotReloadStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("ShaderHotReload")->GetBaseDir() / TEXT("Resources"));

	Style->Set("ShaderHotReload.PluginAction", new IMAGE_BRUSH(TEXT("Icon20"), Icon20x20));
	return Style;
}

void FShaderHotReloadStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FShaderHotReloadStyle::Get()
{
	return *StyleInstance;
}
