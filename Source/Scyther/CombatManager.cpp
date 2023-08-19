// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatManager.h"
#include <Scyther/Enemies/BaseEnemy.h>
#include <Kismet/GameplayStatics.h>
#include <Scyther/ScytherGameModeBase.h>
#include <Scyther/Player/ScytherPlayerPawn.h>
#include <Scyther/Enemies/BaseEnemyCtrl.h>

// Sets default values
ACombatManager::ACombatManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACombatManager::BeginPlay()
{
	Super::BeginPlay();

	gm = Cast<AScytherGameModeBase>( UGameplayStatics::GetGameMode( GetWorld() ) );
}

// Called every frame
void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, FString( "COMBAT" ) );
	for( ABaseEnemyCtrl* ene : inCombatEnemies )
	{
		GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, ene->GetName() );
	}
	GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, FString("COMBAT"));

	GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, FString( "IDLE" ) );
	for( ABaseEnemyCtrl* ene : idleEnemies )
	{
		GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, ene->GetName() );
	}
	GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, FString( "IDLE" ) );

	GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, FString( "GOINGTOIDLE" ) );
	for( ABaseEnemyCtrl* ene : goingToIdleEnemies )
	{
		GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, ene->GetName() );
	}
	GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, FString( "GOINGTOIDLE" ) );
		
}

void ACombatManager::addEnemyToIdleList( ABaseEnemy* enemyToAdd, bool addToGoingToIdle )
{
	ABaseEnemyCtrl* ctrl = Cast<ABaseEnemyCtrl>( enemyToAdd->GetController() );

	if( addToGoingToIdle )
	{
		if( !goingToIdleEnemies.Contains( ctrl ) )
		{
			goingToIdleEnemies.Add( ctrl );
			ctrl->changeCombatState( combatState::goingToIdle );
		}
	}
	else
	{
		if( goingToIdleEnemies.Contains( ctrl ) )
		{
			goingToIdleEnemies.Remove( ctrl );
		}

		if( !idleEnemies.Contains( ctrl ) )
		{
			idleEnemies.Add( ctrl );
			ctrl->changeCombatState( combatState::idle );
		}
	}
	
}

void ACombatManager::refreshInCombatList()
{
	SortIdleByDistance();
	TArray<ABaseEnemyCtrl*> toRemove;

	for( ABaseEnemyCtrl* enemy : idleEnemies )
	{
		if( enemy->combatPoolSize + currentCombatPoolSize <= inCombatMaxPoolSize )
		{
			inCombatEnemies.Push( enemy );
			toRemove.Push( enemy );

			currentCombatPoolSize += enemy->combatPoolSize;
			enemy->changeCombatState( combatState::goingToCombat );
		}
	}

	for( ABaseEnemyCtrl* enemy : toRemove )
	{
		idleEnemies.Remove( enemy );
	}
	
}

void ACombatManager::SortIdleByDistance()
{
	APawn* targetPawn = Cast<APawn>( UGameplayStatics::GetActorOfClass( GetWorld(), gm->aiPlayerBPClass ) );
	if( targetPawn == nullptr )
	{
		targetPawn = UGameplayStatics::GetPlayerPawn( GetWorld(), 0 );
	}

	FVector pawnLocation = targetPawn->GetActorLocation();
	pawnLocation.Z = 0;

	idleEnemies.Sort( [pawnLocation]
	(const ABaseEnemyCtrl& ctrl1,const ABaseEnemyCtrl& ctrl2 )
	{
		return  ctrl1.enemyPawn->getSqrDistanceToLocation( pawnLocation ) < ctrl2.enemyPawn->getSqrDistanceToLocation( pawnLocation ); // or ip1-> or even *ip->
	}
	);
}

void ACombatManager::MaxNumberOfAttacksCompleted( ABaseEnemyCtrl* ctrlToIdle)
{	
	if( idleEnemies.Num() == 0 )
	{
		ctrlToIdle->changeCombatState( combatState::inCombat );
	}
	else
	{
		currentCombatPoolSize -= ctrlToIdle->combatPoolSize;

		inCombatEnemies.Remove( ctrlToIdle );
		ctrlToIdle->changeCombatState( combatState::goingToIdle );
		goingToIdleEnemies.Add( ctrlToIdle );

		refreshInCombatList();
	}
	
}

void ACombatManager::RemoveFromAllLists( ABaseEnemyCtrl* ctrlToRemove )
{
	if( idleEnemies.Contains( ctrlToRemove ) )
	{
		idleEnemies.Remove( ctrlToRemove );
	}
	if( goingToIdleEnemies.Contains( ctrlToRemove ) )
	{
		goingToIdleEnemies.Remove( ctrlToRemove );
	}
	if( inCombatEnemies.Contains( ctrlToRemove ) )
	{
		inCombatEnemies.Remove( ctrlToRemove );
	}
}
