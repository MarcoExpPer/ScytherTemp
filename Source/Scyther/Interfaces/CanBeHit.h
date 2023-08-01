// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CanBeHit.generated.h"

// This class does not need to be modified.
UINTERFACE( BlueprintType )
class UCanBeHit : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SCYTHER_API ICanBeHit
{
	GENERATED_BODY()

public:
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.


	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "Damage interface" )
	void DoDirectDamage(float damage);

};
