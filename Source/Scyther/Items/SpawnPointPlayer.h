// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "SpawnPointPlayer.generated.h"

UCLASS()
class SCYTHER_API ASpawnPointPlayer : public AActor
{
	GENERATED_BODY()

	UPROPERTY( VisibleAnywhere, Category = BoxCollision )
		UBoxComponent* SpawnPointCollider;

	FTransform CurrentCollider;
	
public:	
	// Sets default values for this actor's properties
	ASpawnPointPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	void OverlapSpawnPoint();

};
