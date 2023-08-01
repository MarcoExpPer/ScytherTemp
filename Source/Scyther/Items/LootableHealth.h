// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LootableItem.h"
#include "LootableHealth.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API ALootableHealth: public ALootableItem
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Lootable")
		float healthAmount = 10;

	void setHealthParameters( float newAmount );
protected:
	virtual void playerMeshOverlap( class AScytherPlayerPawn* player ) override;
};
