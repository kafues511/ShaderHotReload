// Copyright © 2025-2026 kafues511 All Rights Reserved.

#include "ShaderHotReloadModule.h"
#include "Engine/Engine.h"
#include "Editor.h"
#include "ShaderCompiler.h"
#include "ToolMenus.h"
#include "HAL/IConsoleManager.h"
#include "Misc/MessageDialog.h"

#include "ShaderHotReloadStyle.h"
#include "ShaderHotReloadCommands.h"

#define LOCTEXT_NAMESPACE "FShaderHotReloadModule"

DEFINE_LOG_CATEGORY(LogShaderHotReload);

void FShaderHotReloadModule::StartupModule()
{
	FShaderHotReloadStyle::Initialize();
	FShaderHotReloadStyle::ReloadTextures();

	FShaderHotReloadCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FShaderHotReloadCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FShaderHotReloadModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FShaderHotReloadModule::RegisterMenus));
}

void FShaderHotReloadModule::ShutdownModule()
{
	if (GEditor && CheckTimerHandle.IsValid())
	{
		GEditor->GetTimerManager()->ClearTimer(CheckTimerHandle);
	}

	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	FShaderHotReloadStyle::Shutdown();
	FShaderHotReloadCommands::Unregister();
}

void FShaderHotReloadModule::PluginButtonClicked()
{
	if (bIsHotReloading)
	{
		// 既に実行中の場合は多重起動になるからダメ
		UE_LOG(LogShaderHotReload, Warning, TEXT("Shader Hot Reload is already in progress."));
		return;
	}

	auto HotReloadCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.HotReload"));
	if (!HotReloadCVar)
	{
		// 描画機能を止める処理が組まれていない場合は実行不可
		UE_LOG(LogShaderHotReload, Error, TEXT("Console variable 'r.HotReload' not found. Engine modification is required."));

		FMessageDialog::Open(
			EAppMsgType::Ok, 
			LOCTEXT(
				"PluginButtonClicked",
				"Cannot execute Shader Hot Reload.\n"
				"\n"
				"The console variable 'r.HotReload' was not found.\n"
				"This plugin requires a custom engine modification to work properly."));
		return;
	}

	UE_LOG(LogShaderHotReload, Log, TEXT("Try ShaderHotReload"));

	bIsHotReloading = true;
	bHasStartedCompiling = false;
	HotReloadStartTime = FPlatformTime::Seconds();

	// 描画機能を一時的に止めてシェーダーの不一致を防止
	HotReloadCVar->Set(1, ECVF_SetByCode);

	GEditor->GetTimerManager()->SetTimerForNextTick(FTimerDelegate::CreateLambda([]()
	{
		if (GEngine)
		{
			GEngine->Exec(nullptr, TEXT("ReloadGlobalShaders"));
		}
	}));

	// コンパイル状態の監視開始
	GEditor->GetTimerManager()->SetTimer(CheckTimerHandle, FTimerDelegate::CreateRaw(this, &FShaderHotReloadModule::CheckShaderCompilationState), 0.1f, true);
}

void FShaderHotReloadModule::CheckShaderCompilationState()
{
	// シェーダーコンパイル中か
	auto bIsCompiling = GShaderCompilingManager ? GShaderCompilingManager->IsCompiling() : false;

	if (bIsCompiling)
	{
		if (!bHasStartedCompiling)
		{
			// コンパイル始まった
			UE_LOG(LogShaderHotReload, Log, TEXT("Shader compilation started..."));
			bHasStartedCompiling = true;
		}
	}
	else
	{
		auto ElapsedTime = FPlatformTime::Seconds() - HotReloadStartTime;

		if (bHasStartedCompiling)
		{
			// コンパイル完了ないし成功
			UE_LOG(LogShaderHotReload, Log, TEXT("Completed ShaderHotReload successfully."));
			RestoreRendering();
		}
		else if (ElapsedTime > 2.0) 
		{
			// 2秒待ってもコンパイルが始まらない場合はタイムアウトとして終了
			UE_LOG(LogShaderHotReload, Log, TEXT("No shader changes detected or compilation timeout."));
			RestoreRendering();
		}
	}
}

void FShaderHotReloadModule::RestoreRendering()
{
	if (GEditor && CheckTimerHandle.IsValid())
	{
		// 監視タイマを停止
		GEditor->GetTimerManager()->ClearTimer(CheckTimerHandle);
		CheckTimerHandle.Invalidate();
	}

	if (auto HotReloadCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.HotReload")))
	{
		// 描画処理を元に戻す
		HotReloadCVar->Set(0, ECVF_SetByCode);
	}

	bIsHotReloading = false;
	bHasStartedCompiling = false;

	UE_LOG(LogShaderHotReload, Log, TEXT("RestoreRendering: r.HotReload set to false."));
}

void FShaderHotReloadModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FShaderHotReloadCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FShaderHotReloadCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FShaderHotReloadModule, ShaderHotReload)
