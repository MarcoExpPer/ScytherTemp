// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Enemies/BaseEnemyCtrl.h"
#include "Items/ManadropPool.h"
#include "ScytherGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API AScytherGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
		//Variables
public:
	//-- POOLS --//
	UPROPERTY( EditAnywhere, BlueprintReadWrite)
	AManadropPool *manaPool;

	//-- MANAGERS --//
	UPROPERTY( EditAnywhere, BlueprintReadWrite)
	class ARespawnManager *respawnMan;

private:
	FTransform spawnPoint;
	TArray<ABaseEnemyCtrl*> enemyList;



		//Functions
public:
		void BeginPlay();

		UFUNCTION( BlueprintCallable, Category = "Respawn", meta = (
			ToolTip = "Actualiza el punto de spawn del jugador. Si deleteEnemyRespawnList esta a true, tambien se limpia la lista de enemigos a respawnear" ) )
			void updateSpawnPoint( FTransform newSpawnPoint, bool deleteEnemyRespawnList = true );

		UFUNCTION( BlueprintCallable, Category = "Respawn", meta = (
			ToolTip = "Devuelve el spawn point actual" ) )
			FTransform getSpawnPoint();

		UFUNCTION( BlueprintCallable, Category = "Respawn", meta = (
			ToolTip = "Vuelve a spawnear todos los enemigos guardados en la lista" ) )
			void respawnAllEnemies();

		UFUNCTION( BlueprintCallable, Category = "Respawn", meta = (
			ToolTip = "Vuelve a spawnear todos los enemigos guardados en la lista" ) )
			void addEnemyToTheList( ABaseEnemyCtrl* enemyToAdd );
	
};
