// Copyright Cybertooth Studio LLC. All Rights Reserved.

#include "ReselectCommands.h"
#include "Framework/Commands/UICommandInfo.h"

#define LOCTEXT_NAMESPACE "FReselectPluginModule"

void FReselectCommands::RegisterCommands()
{
	UI_COMMAND(ResetCameraLocation, "Reset Camera", "Returns viewport camera to position before \"Play\"", EUserInterfaceActionType::ToggleButton, FInputChord());
}

#undef LOCTEXT_NAMESPACE
