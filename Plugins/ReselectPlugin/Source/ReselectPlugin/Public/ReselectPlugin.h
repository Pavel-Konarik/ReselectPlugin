// Copyright Cybertooth Studio LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "LevelEditor.h"

/**
 * This Plugin handles 2 things
 *	  - Reselect actors after "Play In Editor" exits
 *	  - Moves camera back to starting position after "Play In Editor" exits
 *
 * Reselecting actors is straight forward, we simply store pointer to actors before PIE starts,
 * and on PIE End we reselect those actors
 *
 * Moving camera back requires EndPlayMap event instead of EndPIE, because UE moves
 * viewport camera to "End-game" camera position after ENDPIE is called.
 * EndPlayMap is called even after that.
 * Additionally, it was required to find a correct viewport that "Play" was happening at.
 *
 * Rest of the functions mostly add a button to "Play combo" box that enables/disables camera reset.
 */

class FReselectPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


	// Begin PIE delegate
	void OnPiePreStart(bool bSimulate);
	void OnPieEnd(bool bSimulate);
	void OnEndPlayMap();


	// Camera return to original location - Menu button
	TSharedRef<FExtender> OnExtendLevelEditorBuildMenu(const TSharedRef<FUICommandList> CommandList);
	void CreateCameraResetOptionMenu(FMenuBuilder& Builder);
	static void OnCameraReturnToggle();
	static bool IsCameraReturnEnabled();

	class FLevelEditorViewportClient* GetFirstActiveLevelViewportClient();

private:
	TSharedPtr<class FUICommandList> PluginCommands;

	TArray<class AActor*> SelectedActors;

	FDelegateHandle OnBeginPIEHandle;
	FDelegateHandle OnEndPIEHandle;

	// Camera 
	class FLevelEditorViewportClient* TargetViewport;
	FVector StartingViewLocation;
	FRotator StartingViewRotation;

	FLevelEditorModule::FLevelEditorMenuExtender CameraResetOptionExtender;

	FDelegateHandle CameraResetOptionExtenderHandle;


};
