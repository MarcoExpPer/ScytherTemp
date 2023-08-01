// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CanEnterDarkLightAreas.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCanEnterDarkLightAreas : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SCYTHER_API ICanEnterDarkLightAreas
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "Balance Areas" )
		void EnterDarkArea();

	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "Balance Areas" )
		void EnterLightArea();

	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "Balance Areas" )
		void SkillToLightMode();

	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "Balance Areas" )
		void SkillToDarkMode();

	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "Balance Areas" )
		void SkillEnds();
};
