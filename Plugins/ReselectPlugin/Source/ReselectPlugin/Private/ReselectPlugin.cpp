// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReselectPlugin.h"
#include "Engine/Selection.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "FReselectPluginModule"

void FReselectPluginModule::StartupModule()
{
	FEditorDelegates::PreBeginPIE.AddRaw(this, &FReselectPluginModule::OnPieEvent);
	FEditorDelegates::EndPIE.AddRaw(this, &FReselectPluginModule::OnPieEnd);
}

void FReselectPluginModule::ShutdownModule()
{

}


void FReselectPluginModule::OnPieEvent(const bool bSimulate)
{
	if (GEditor) {
		USelection* Selection = GEditor->GetSelectedActors();
		for (int32 i = 0; i < Selection->Num(); ++i)
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

		for (int32 i = 0; i < SelectedActors.Num(); ++i)
		{
			if(IsValid(SelectedActors[i]))
			{
				GEditor->SelectActor(SelectedActors[i], true, true, false, false);
			}
		}

	}
	SelectedActors.Empty();
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FReselectPluginModule, ReselectPlugin)