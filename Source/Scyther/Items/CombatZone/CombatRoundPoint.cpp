// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatRoundPoint.h"
#include <Scyther/Enemies/BaseEnemy.h>
#include <Scyther/Components/HealthComponent.h>
#include <AIController.h>

// Sets default values
ACombatRoundPoint::ACombatRoundPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OnDestroyed.AddDynamic( this, &ACombatRoundPoint::OnActorDestroyedDelegate );
}

// Called when the game starts or when spawned
void ACombatRoundPoint::BeginPlay()
{
	Super::BeginPlay();

	for( ABaseEnemy* ePawn : enemiesPawns )
	{
		ePawn->isInCombatZone = true;
		ePawn->bindToRoundStart( this );
	
		FTimerHandle MyTimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda( [ePawn]()
		{
			ePawn->deSpawn();
		} );

		GetWorld()->GetTimerManager().SetTimer( MyTimerHandle, TimerDelegate, 0.5f, false );
	}

	enemyCount = enemiesPawns.Num();
}

// Called every frame
void ACombatRoundPoint::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ACombatRoundPoint::SpawnEnemies()
{
	for( ABaseEnemy* e : enemiesPawns )
	{
		e->spawn( e->initialTransform );
	}

	enemyCount = enemiesPawns.Num();
}

void ACombatRoundPoint::DespawnEnemies()
{
	for( ABaseEnemy* e : enemiesPawns )
	{
		e->deSpawn();
	}

	enemyCount = 0;
}

void ACombatRoundPoint::SingleEnemyDead( )
{
	enemyCount--;

	if( enemyCount <= 0 )
	{
		AllEnemiesDeadEvent.Execute();
	}
}

void ACombatRoundPoint::OnActorDestroyedDelegate( AActor* DestroyedActor )
{
	for( ABaseEnemy* ePawn : enemiesPawns )
	{
		ePawn->Destroy();
	}
}

void ACombatRoundPoint::CombatRoundActivatedBroadcast()
{
	CombatRoundActivated.Broadcast();
}