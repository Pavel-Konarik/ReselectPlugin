// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReselectPlugin.h"
#include "Engine/Selection.h"
#include "Editor.h"
#include "LevelEditorViewport.h"
#include "LevelEditor.h"
#include "IAssetViewport.h"
#include "UnrealClient.h"
#include "SLevelViewport.h"
#include <GameDelegates.h>

#define LOCTEXT_NAMESPACE "FReselectPluginModule"

void FReselectPluginModule::StartupModule()
{
	// Register hooks to start and end of "Play In Editor"
	OnBeginPIEHandle = FEditorDelegates::PreBeginPIE.AddRaw(this, &FReselectPluginModule::OnPiePreStart);
	OnEndPIEHandle = FEditorDelegates::EndPIE.AddRaw(this, &FReselectPluginModule::OnPieEnd);

	FGameDelegates::Get().GetEndPlayMapDelegate().AddRaw(this, &FReselectPluginModule::OnEndPlayMap);
}

void FReselectPluginModule::ShutdownModule()
{
	// Cleanup - Unregister hooks
	FEditorDelegates::PreBeginPIE.Remove(OnBeginPIEHandle);
	FEditorDelegates::EndPIE.Remove(OnEndPIEHandle);
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

void FReselectPluginModule::OnPiePreStart(const bool bSimulate)
{
	bool bShouldResetCamera = true;

	if (bShouldResetCamera) {
		Viewport = GetFirstActiveLevelViewportClient();
		if (Viewport) {
			StartingViewLocation = Viewport->GetViewLocation();
			StartingViewRotation = Viewport->GetViewRotation();
		}

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
	if (Viewport) {
		Viewport->SetViewLocation(StartingViewLocation);
		Viewport->SetViewRotation(StartingViewRotation);
	}
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FReselectPluginModule, ReselectPlugin)