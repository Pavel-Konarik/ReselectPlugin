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
	void OnPieEvent(bool a);

	void OnPieEnd(bool a);

private:
	TArray<class AActor*> SelectedActors;

};
