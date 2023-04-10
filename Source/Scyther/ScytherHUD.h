// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ScytherHUD.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API AScytherHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	UFUNCTION( BlueprintImplementableEvent, category = "HUD" )
		void TogglePauseHUD();

};
