// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

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

	class FLevelEditorViewportClient* GetFirstActiveLevelViewportClient();

private:
	TArray<class AActor*> SelectedActors;


	FDelegateHandle OnBeginPIEHandle;
	FDelegateHandle OnEndPIEHandle;

	class FLevelEditorViewportClient* Viewport;
	FVector StartingViewLocation;
	FRotator StartingViewRotation;

};
