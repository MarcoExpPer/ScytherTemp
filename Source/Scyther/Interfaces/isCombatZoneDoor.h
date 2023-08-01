// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "isCombatZoneDoor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UisCombatZoneDoor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SCYTHER_API IisCombatZoneDoor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "CombatDoor Areas" )
		void OpenCombatZoneDoor();

	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "CombatDoor Areas" )
		void CloseCombatZoneDoor();

	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "CombatDoor Areas" )
		void PrepareCombatZoneDoor();
};
