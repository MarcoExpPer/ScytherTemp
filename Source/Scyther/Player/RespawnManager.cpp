


#include "RespawnManager.h"
#include "Kismet/GameplayStatics.h"
#include "Scyther/Components/HealthComponent.h"
#include "Scyther/Components/SkillComponent.h"
#include "ScytherPlayerPawn.h"
#include "../Enemies/BaseEnemy.h"
#include "../Enemies/BaseEnemyCtrl.h"
#include <Scyther/Interfaces/CanRespawn.h>



// Sets default values
ARespawnManager::ARespawnManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARespawnManager::BeginPlay()
{
	Super::BeginPlay();

	Cast<AScytherPlayerPawn>( UGameplayStatics::GetPlayerPawn(GetWorld(), 0) )->healthComponent->zerohealthEvent.AddDynamic(this, &ARespawnManager::IsDead );
	LastPlayerSpawnPosition = UGameplayStatics::GetPlayerPawn( GetWorld(), 0 )->GetTransform().GetLocation();

	playerControl = UGameplayStatics::GetPlayerController( GetWorld(), 0 );
}

// Called every frame
void ARespawnManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ARespawnManager::PushSpawnPoint( FTransform spawnPoint )
{
	//spawnPoints = TArray<FTransform>();
	if( SpawnPoints.Num() )
	{
		spawnPoint.SetScale3D( FVector( 1.f, 1.f, 1.f ) );
		SpawnPoints.Add( spawnPoint );

		newCheckpointEvent.Broadcast( spawnPoint.GetLocation() );
	}

	ClearRespawnUntilCheckpointList();
}

FTransform ARespawnManager::PopSpawnPoint()
{
	if( SpawnPoints.Num() - 1 )
	{
		RemovedSpawnPoint = SpawnPoints[0];
		SpawnPoints.RemoveAt( 0 );
	}
	return RemovedSpawnPoint;
}

FTransform ARespawnManager::GetSpawnPoint()
{
	if( SpawnPoints.Num() - 1 )
	{
		return SpawnPoints[0];
	}
	return SpawnPoints[0];
}

void ARespawnManager::IsDead( DamageModes damageType )
{
	playerDeadEvent.Broadcast();

	waitingForRespawn = true;
	playerControl->bForceFeedbackEnabled = false;

	playerControl->SetPause(true);
}

void ARespawnManager::RestartController()
{
	waitingForRespawn = false;
	playerControl->bForceFeedbackEnabled = true;
	playerControl->SetPause( false );

	RespawnAlwaysRespawnList();
	RespawnRespawnUntilCheckpointList();

	UE_LOG( LogTemp, Warning, TEXT( "Current TestPushSpawnPoint:: %s" ), *LastPlayerSpawnPosition.ToString() );
	const FVector LocationSpawn = SpawnPoints[0].GetLocation();

	AScytherPlayerPawn* player = Cast<AScytherPlayerPawn>( UGameplayStatics::GetPlayerPawn( GetWorld(), 0 ) );
	player->healthComponent->changeHealth( player->healthComponent->initialHp, DamageModes::HEALING, true );
	player->healthComponent->saveLastHp = true;
	player->skillComponent->currentLightMana = 0;
	player->skillComponent->UpdateMana( true, manaToRespawnThePlayerWith );
	
	//Broadcast player respawn event
	playerRespawnEvent.Broadcast( LocationSpawn );

	FHitResult HitResult;
	player->SetActorLocation( LocationSpawn );


	EnableInput( UGameplayStatics::GetPlayerController( GetWorld(), 0 ) );


	if( LastPlayerSpawnPosition == LocationSpawn )
	{
		if( DeadEnemies.Num() != 0 )
		{
			UE_LOG( LogTemp, Warning, TEXT( "CurrentSpawnEnemy:: %u" ), DeadEnemies.Num() );

			for( int i = 0; i < DeadEnemies.Num(); i++ )
			{
				if( DeadEnemies[i]->enemyPawn->GetIsDead() && !DeadEnemies[i]->enemyPawn->GetIsInCombatZone() )
				{
					DeadEnemies[i]->enemyPawn->spawn( DeadEnemies[i]->enemyPawn->initialTransform);
				}
			}
			DeadEnemies.Empty();
		}
	}
}

void ARespawnManager::AddToAlwaysRespawnList(AActor *a)
{
	bool canRespawn = a->GetClass()->ImplementsInterface( UCanRespawn::StaticClass() );
	check( canRespawn );

	alwaysRespawnList.Add( a );
}

void ARespawnManager::RespawnAlwaysRespawnList()
{
	for( AActor* a : alwaysRespawnList )
	{
		ICanRespawn::Execute_Respawn( a );
	}

	alwaysRespawnList.Empty();
}

void ARespawnManager::AddToRespawnUntilCheckpointList( AActor* a )
{
	bool canRespawn = a->GetClass()->ImplementsInterface( UCanRespawn::StaticClass() );
	check( canRespawn );

	respawnUntilCheckpointList.Add( a );
}

void ARespawnManager::RespawnRespawnUntilCheckpointList()
{
	for( AActor* a : respawnUntilCheckpointList )
	{
		ICanRespawn::Execute_Respawn( a );
	}

	respawnUntilCheckpointList.Empty();

}

void ARespawnManager::ClearRespawnUntilCheckpointList()
{
	for( AActor* a : respawnUntilCheckpointList )
	{
		if(a != nullptr)
		{
			a->Destroy();
		}
	}
	respawnUntilCheckpointList.Empty();
}







