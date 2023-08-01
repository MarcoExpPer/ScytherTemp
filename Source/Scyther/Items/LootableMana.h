// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LootableItem.h"
#include "LootableMana.generated.h"

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FIsLightManaChanged );

UCLASS()
class SCYTHER_API ALootableMana : public ALootableItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lootable")
		float manaAmount = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lootable")
		bool isLightMana = true;

	UPROPERTY( BlueprintAssignable )
	FIsLightManaChanged IsLightManaChangedEvent;


	void setManaParameters( bool newIsLightMana, float newAmount );

protected:
	virtual void playerMeshOverlap(class AScytherPlayerPawn* player) override;
};
