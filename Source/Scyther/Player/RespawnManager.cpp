


#include "RespawnManager.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Scyther/Components/HealthComponent.h"
#include "ScytherPlayerPawn.h"

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
}

// Called every frame
void ARespawnManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ARespawnManager::PushSpawnPoint( FTransform spawnPoint )
{
	//spawnPoints = TArray<FTransform>();
	if( spawnPoints.Num() )
	{
		spawnPoint.SetScale3D( FVector( 1.f, 1.f, 1.f ) );
		spawnPoints.Add( spawnPoint );

	}

}

FTransform ARespawnManager::PopSpawnPoint()
{
	if( spawnPoints.Num() - 1 )
	{
		removedSpawnPoint = spawnPoints[0];
		spawnPoints.RemoveAt( 0 );
	}
	return removedSpawnPoint;
}

FTransform ARespawnManager::GetSpawnPoint()
{
	if( spawnPoints.Num() - 1 )
	{
		return spawnPoints[0];
	}
	return spawnPoints[0];
}

void ARespawnManager::IsDead( ElementTypes damageType )
{
	DisableInput( UGameplayStatics::GetPlayerController( GetWorld(), 0 ) );

	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(
		UnusedHandle, this, &ARespawnManager::RestartController, 0.5f, false );
}

void ARespawnManager::RestartController()
{
	AScytherPlayerPawn* player = Cast<AScytherPlayerPawn>(UGameplayStatics::GetPlayerPawn( GetWorld(), 0 ));
	
	player->healthComponent->changeHealth(100.f,ElementTypes::NEUTRAL);

	//Broadcast player respawn event
	playerRespawnEvent.Broadcast();

	const FVector LocationSpawn = spawnPoints[0].GetLocation();
	FHitResult HitResult;
	player->K2_SetActorLocation( LocationSpawn , false , HitResult , false);

	EnableInput( UGameplayStatics::GetPlayerController( GetWorld(), 0 ) );
}




