// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Components/HealthComponent.h"
#include "RespawnManager.generated.h"

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FPlayerRespawn );

UCLASS()
class SCYTHER_API ARespawnManager: public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARespawnManager();

	UPROPERTY( BlueprintAssignable )
	FPlayerRespawn playerRespawnEvent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

	void PushSpawnPoint( FTransform spawnPoint );

	FTransform PopSpawnPoint();

	FTransform GetSpawnPoint();

	UFUNCTION()
	void IsDead( ElementTypes damageType);

	void RestartController();

	UPROPERTY( EditAnywhere )
		TArray<FTransform> spawnPoints;
	
	UPROPERTY( Transient, EditAnywhere, BlueprintReadWrite, Category = "Camera" )
		AActor* CameraPlayer;
	//LocalVariable
	FTransform removedSpawnPoint;
	

};
