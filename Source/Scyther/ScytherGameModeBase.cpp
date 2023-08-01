// Fill out your copyright notice in the Description page of Project Settings.


#include "ScytherGameModeBase.h"
#include "Player/RespawnManager.h"
#include <Kismet/GameplayStatics.h>
#include "Items/LootableItem.h"
#include "Items/ManadropPool.h"

void AScytherGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters spawn;
	manaPool = GetWorld()->SpawnActor<AManadropPool>( spawn );
	respawnMan = GetWorld()->SpawnActor<ARespawnManager>( spawn );

	respawnMan->SpawnPoints.Add( UGameplayStatics::GetPlayerPawn( GetWorld(), 0 )->GetTransform() );
}

void AScytherGameModeBase::updateSpawnPoint( FTransform newSpawnPoint, bool deleteEnemyRespawnList )
{
	spawnPoint = newSpawnPoint;

	if( deleteEnemyRespawnList )
	{
		enemyList.Empty( 5 );
	}
}


FTransform AScytherGameModeBase::getSpawnPoint()
{
	return spawnPoint;
}

void AScytherGameModeBase::respawnAllEnemies()
{
	for( ABaseEnemyCtrl* ctrl : enemyList )
	{
		//ctrl->enemyPawn->spawn( FVector( 0, 0, 0 ) );
	}
}

void AScytherGameModeBase::addEnemyToTheList( ABaseEnemyCtrl* enemyToAdd )
{
	enemyList.Add( enemyToAdd );
}