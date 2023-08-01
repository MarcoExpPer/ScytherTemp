// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Components/HealthComponent.h"
#include "RespawnManager.generated.h"

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FPlayerRespawn, FVector, location );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FNewCheckpoint, FVector, location );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FPlayerDead  );

UCLASS()
class SCYTHER_API ARespawnManager: public AActor
{
	GENERATED_BODY()

private:
	APlayerController* playerControl;
public:
	// Sets default values for this actor's properties
	ARespawnManager();

	UPROPERTY( BlueprintAssignable )
		FPlayerRespawn playerRespawnEvent;

	UPROPERTY( BlueprintAssignable )
		FNewCheckpoint newCheckpointEvent;

	UPROPERTY( BlueprintAssignable )
		FPlayerDead playerDeadEvent;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	float manaToRespawnThePlayerWith = 0;

	// Called every frame
	virtual void Tick( float DeltaTime ) override;

	void PushSpawnPoint( FTransform spawnPoint );

	FTransform PopSpawnPoint();

	FTransform GetSpawnPoint();

	UFUNCTION()
		void IsDead( DamageModes damageType );

	void RestartController();


	UPROPERTY( EditAnywhere )
		TArray<FTransform> SpawnPoints;
	UPROPERTY( VisibleAnywhere, Category = "EnemyControllers" )
		TArray<class ABaseEnemy*> EnemiesPos;

	UPROPERTY( VisibleAnywhere, Category = "DeadEnemyTracker" )
		TArray<class ABaseEnemyCtrl*> DeadEnemies;

	UPROPERTY( Transient, EditAnywhere, BlueprintReadWrite, Category = "Camera" )
		AActor* CameraPlayer;
	//LocalVariable
	FTransform RemovedSpawnPoint;
	FVector LastPlayerSpawnPosition;

	bool waitingForRespawn = false;

	/// <summary>
	/// Actors in this list will always respawn, even if the player gets to a checkpoint, they wont be destroyed.
	/// </summary>
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
		TArray<class AActor*> alwaysRespawnList;

	UFUNCTION( BlueprintCallable )
		void AddToAlwaysRespawnList( AActor* a );

	void RespawnAlwaysRespawnList();


	/// <summary>
	/// Actors in this list will respawn when the player respawns but if the player gets to a checkpoint, they will stop respawning and will be destroyed
	/// </summary>
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
		TArray<class AActor*> respawnUntilCheckpointList;

	UFUNCTION( BlueprintCallable )
		void AddToRespawnUntilCheckpointList( AActor* a );

	void RespawnRespawnUntilCheckpointList();
	void ClearRespawnUntilCheckpointList();



};
