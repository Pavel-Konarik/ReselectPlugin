// Copyright Cybertooth Studio LLC. All Rights Reserved.

#include "ReselectPlugin.h"
#include "Engine/Selection.h"
#include "Editor.h"
#include "LevelEditorViewport.h"
#include "LevelEditor.h"
#include "IAssetViewport.h"
#include "UnrealClient.h"
#include "SLevelViewport.h"
#include "GameDelegates.h"
#include "ISettingsModule.h"
#include "ReselectSettings.h"
#include "ReselectCommands.h"

#define LOCTEXT_NAMESPACE "FReselectPluginModule"

void FReselectPluginModule::StartupModule()
{
	// Register hooks to start and end of "Play In Editor"
	OnBeginPIEHandle = FEditorDelegates::PreBeginPIE.AddRaw(this, &FReselectPluginModule::OnPiePreStart);
	OnEndPIEHandle = FEditorDelegates::EndPIE.AddRaw(this, &FReselectPluginModule::OnPieEnd);
	FGameDelegates::Get().GetEndPlayMapDelegate().AddRaw(this, &FReselectPluginModule::OnEndPlayMap);

	// Register and map commands
	FReselectCommands::Register();
	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FReselectCommands::Get().ResetCameraLocation,
		FExecuteAction::CreateStatic(&FReselectPluginModule::OnCameraReturnToggle),
		FCanExecuteAction(),
		FIsActionChecked::CreateStatic(&FReselectPluginModule::IsCameraReturnEnabled));

	// Add button to toggle Camera return to original location
	// in "Play" Combo button
	FLevelEditorModule* LevelEditorModule = FModuleManager::LoadModulePtr<FLevelEditorModule>(TEXT("LevelEditor"));
	if (LevelEditorModule)
	{
		CameraResetOptionExtender = FLevelEditorModule::FLevelEditorMenuExtender::CreateRaw(this, &FReselectPluginModule::OnExtendLevelEditorBuildMenu);

		TArray<FLevelEditorModule::FLevelEditorMenuExtender>& MenuExtenders = LevelEditorModule->GetAllLevelEditorToolbarPlayMenuExtenders();
		MenuExtenders.Add(CameraResetOptionExtender);
		// Remember Handle so we know what extender to unload during shutdown of this module
		CameraResetOptionExtenderHandle = MenuExtenders.Last().GetHandle();
	}

	// Register plugin settings 
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings(
			"Editor", "Plugins", "Reselect",
			LOCTEXT("PluginName", "Reselect"),
			LOCTEXT("PluginDesc", "Configure settings for Reselect Plugin"),
			GetMutableDefault<UReselectSettings>()
		);
	}
}

void FReselectPluginModule::ShutdownModule()
{
	// Cleanup - Unregister hooks
	FEditorDelegates::PreBeginPIE.Remove(OnBeginPIEHandle);
	FEditorDelegates::EndPIE.Remove(OnEndPIEHandle);

	// Remove custom settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Editor", "Plugins", "Reselect");
	}

	// Remove Camera reset location button
	FLevelEditorModule* LevelEditorModule = FModuleManager::LoadModulePtr<FLevelEditorModule>(TEXT("LevelEditor"));
	if (LevelEditorModule)
	{
		LevelEditorModule->GetAllLevelEditorToolbarPlayMenuExtenders().RemoveAll([=](const FLevelEditorModule::FLevelEditorMenuExtender& Extender) { return Extender.GetHandle() == CameraResetOptionExtenderHandle; });
	}

	FReselectCommands::Unregister();
}

void FReselectPluginModule::OnPiePreStart(const bool bSimulate)
{
	// Remember the viewport pointer, camera location and rotation
	TargetViewport = GetFirstActiveLevelViewportClient();
	if (TargetViewport) {
		StartingViewLocation = TargetViewport->GetViewLocation();
		StartingViewRotation = TargetViewport->GetViewRotation();
	}


	// Get all currently selected actors and remember them
	if (GEditor) {
		USelection* Selection = GEditor->GetSelectedActors();
		for (int32 i = 0; i < Selection->Num(); i++)
		{
			AActor* SelectedActor = Cast<AActor>(Selection->GetSelectedObject(i));
			if (IsValid(SelectedActor)) {
				SelectedActors.Add(SelectedActor);
			}
		}
	}
}

void FReselectPluginModule::OnPieEnd(const bool bSimulate)
{
	// 
	if (GEditor) {
		TArray<AActor*> ValidActors;

		for (int32 i = 0; i < SelectedActors.Num(); i++)
		{
			if(IsValid(SelectedActors[i]))
			{
				GEditor->SelectActor(SelectedActors[i], true, true, false, false);
			}
		}
	}
	SelectedActors.Empty();
}

void FReselectPluginModule::OnEndPlayMap() 
{
	// This triggers after OnPieEnd, after viewport camera is moved to game camera
	bool bShouldResetCamera = GetDefault<UReselectSettings>()->bShouldReturnCamera;

	if (bShouldResetCamera && TargetViewport) {
		TargetViewport->SetViewLocation(StartingViewLocation);
		TargetViewport->SetViewRotation(StartingViewRotation);
	}
}



void FReselectPluginModule::CreateCameraResetOptionMenu(FMenuBuilder& Builder)
{
	Builder.BeginSection("Reselect", LOCTEXT("ReselectPlayComboSection", "On Play End"));

	Builder.AddMenuEntry(FReselectCommands::Get().ResetCameraLocation);

	Builder.EndSection();
}


TSharedRef<FExtender> FReselectPluginModule::OnExtendLevelEditorBuildMenu(const TSharedRef<FUICommandList> CommandList)
{
	TSharedRef<FExtender> Extender(new FExtender());

	Extender->AddMenuExtension("LevelEditorPlayInWindowNetwork", EExtensionHook::After, PluginCommands,
		FMenuExtensionDelegate::CreateRaw(this, &FReselectPluginModule::CreateCameraResetOptionMenu));

	return Extender;
}


void FReselectPluginModule::OnCameraReturnToggle()
{
	GetMutableDefault<UReselectSettings>()->bShouldReturnCamera = !GetDefault<UReselectSettings>()->bShouldReturnCamera;
}

bool FReselectPluginModule::IsCameraReturnEnabled()
{
	return GetDefault<UReselectSettings>()->bShouldReturnCamera;
}

FLevelEditorViewportClient* FReselectPluginModule::GetFirstActiveLevelViewportClient()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	TSharedPtr<SLevelViewport> ActiveLevelViewport = LevelEditorModule.GetFirstActiveLevelViewport();

	for (FLevelEditorViewportClient* LevelVC : GEditor->GetLevelViewportClients())
	{
		if (LevelVC->Viewport == ActiveLevelViewport->GetActiveViewport()) {
			return LevelVC;
		}
	}

	return nullptr;
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FReselectPluginModule, ReselectPlugin)