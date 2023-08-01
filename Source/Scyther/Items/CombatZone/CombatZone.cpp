// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatZone.h"
#include <Components/BoxComponent.h>
#include <Scyther/Interfaces/isCombatZoneDoor.h>
#include "./CombatRoundPoint.h"
#include <Scyther/ScytherGameModeBase.h>
#include <Scyther/Player/RespawnManager.h>
#include <Kismet/GameplayStatics.h>

// Sets default values
ACombatZone::ACombatZone()
{
	PrimaryActorTick.bCanEverTick = true;

	mainTrigger = CreateDefaultSubobject<UBoxComponent>( TEXT( "Combat Trigger" ) );
	mainTrigger->SetupAttachment( RootComponent );

	mainTrigger->OnComponentBeginOverlap.AddDynamic( this, &ACombatZone::OnCombatTriggerOverlap );

	OnDestroyed.AddDynamic( this, &ACombatZone::OnActorDestroyedDelegate );
}

void ACombatZone::BeginPlay()
{
	Super::BeginPlay();

	ValidateDoors();

	PrepareDoors();

	isCombatActive = false;

	if( firstRoundStartsSpawned )
	{
		GetWorldTimerManager().SetTimer(
			   startRoundHandle, this, &ACombatZone::StartRound, 2.f, false );
	}
}

// Called every frame
void ACombatZone::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ACombatZone::OnCombatTriggerOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	if( !isCombatActive && currentRound == 0 && !externalActivation)
	{
		if( OtherComp->ComponentHasTag( "PlayerBody" ) )
		{
			startCombat();
		}
	}
}

void ACombatZone::startCombat()
{
	isCombatActive = true;
	CloseDoors();
	CombatStartsEvent.Broadcast();

	if( !firstRoundStartsSpawned )
	{
		GetWorldTimerManager().SetTimer(
			startRoundHandle, this, &ACombatZone::StartRound, delayBeforeFirstRound, false );
	}
	else
	{
		rounds[currentRound]->CombatRoundActivatedBroadcast();
	}
}

void ACombatZone::StartRound()
{
	if( currentRound > 0 || (currentRound == 0 && !firstRoundStartsSpawned) )
	{
		rounds[currentRound]->CombatRoundActivatedBroadcast();
	}

	rounds[currentRound]->SpawnEnemies();
	rounds[currentRound]->AllEnemiesDeadEvent.BindLambda( [&]() -> void
	{
		this->roundFinished();
	} );
}

void ACombatZone::roundFinished()
{
	currentRound++;

	if( currentRound == rounds.Num() )
	{
		endCombat( autoRespawnOnCombatEnd );
	}
	else
	{
		CombatRoundFinished.Broadcast();

		
		GetWorldTimerManager().SetTimer(
			startRoundHandle, this, &ACombatZone::StartRound, delayBetweenRounds, false );
	}
}

void ACombatZone::endCombat( bool allowAutoRespawn )
{
	OpenDoors();
	isCombatActive = false;
	CombatEndsEvent.Broadcast();

	if( autoRespawnOnCombatEnd && allowAutoRespawn )
	{
		respawnCombatZone();

		GetWorldTimerManager().SetTimer(
			   startRoundHandle, this, &ACombatZone::StartRound, delayBetweenRounds, false );
	}
	else
	{
		GetWorldTimerManager().ClearTimer( startRoundHandle );
		startRoundHandle.Invalidate();

		for( ACombatRoundPoint* r : rounds )
		{
			r->DespawnEnemies();
		}
	}
}

void ACombatZone::CloseDoors()
{
	for( AActor* d : doors )
	{
		IisCombatZoneDoor::Execute_CloseCombatZoneDoor( d );
	}
}

void ACombatZone::OpenDoors()
{
	for( AActor* d : doors )
	{
		IisCombatZoneDoor::Execute_OpenCombatZoneDoor( d );
	}
}

void ACombatZone::PrepareDoors()
{
	for( AActor* d : doors )
	{
		IisCombatZoneDoor::Execute_PrepareCombatZoneDoor( d );
	}
}

void ACombatZone::ValidateDoors()
{
	for( AActor* d : doors )
	{
		if( d != nullptr )
		{
			bool isCombatZoneDoor = d->GetClass()->ImplementsInterface( UisCombatZoneDoor::StaticClass() );
			check( isCombatZoneDoor );
		}
	}
}

void ACombatZone::respawnCombatZone()
{
	PrepareDoors();

	if( startRoundHandle.IsValid() )
	{
		GetWorldTimerManager().ClearTimer( startRoundHandle );
		startRoundHandle.Invalidate();
	}

	isCombatActive = false;

	currentRound = 0;

	for( ACombatRoundPoint* r : rounds )
	{
		r->DespawnEnemies();
	}

	if( firstRoundStartsSpawned )
	{
		GetWorldTimerManager().SetTimer(
			   startRoundHandle, this, &ACombatZone::StartRound, 2.f, false );
	}
}


void ACombatZone::OnActorDestroyedDelegate( AActor* DestroyedActor )
{
	if( destroyDoorsOnCombatZoneDestroy )
	{
		for( AActor* d : doors )
		{
			d->Destroy();
		}
	}

	for( ACombatRoundPoint* r : rounds )
	{
		r->Destroy();
	}
}