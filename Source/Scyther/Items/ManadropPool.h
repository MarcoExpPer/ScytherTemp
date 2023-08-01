// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ManadropPool.generated.h"

UCLASS()
class SCYTHER_API AManadropPool : public AActor
{
	GENERATED_BODY()
	

//TODO make it work with new LooteableItems
private:
	//UClass* manaDropBPClass;
	//TArray<AManaDrop*> pool;

	//int initialPoolSize = 30;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	//void spawnDrop( FVector location, bool isLightMode, float manaAmount, float expAmount );

private:
	//AManaDrop* createManaDrop();
};
