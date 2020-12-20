// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ReselectSettings.generated.h"

/**
 * 
 */
UCLASS(config = EditorSettings)
class RESELECTPLUGIN_API UReselectSettings : public UObject
{
	GENERATED_BODY()
	

public:
	UReselectSettings(const FObjectInitializer& ObjectInitializer);

	/** If enabled, all newly spawned nodes will have stretching enabled */
	UPROPERTY(EditAnywhere, config, Category = "Defaults")
	bool bShouldReturnCamera;

};
