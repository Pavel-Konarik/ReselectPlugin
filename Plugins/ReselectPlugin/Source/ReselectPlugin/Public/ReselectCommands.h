// Copyright Cybertooth Studio LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include <EditorStyleSet.h>

class FReselectCommands : public TCommands<FReselectCommands>
{
public:

	FReselectCommands()
		: TCommands<FReselectCommands>(TEXT("ReselectPlguin"), NSLOCTEXT("Contexts", "ReselectPlguin", "ReselectPlguin Plugin"), NAME_None, FEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > ResetCameraLocation;
};
