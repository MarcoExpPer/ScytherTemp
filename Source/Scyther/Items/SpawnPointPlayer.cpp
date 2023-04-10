// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPointPlayer.h"
#include "../player/RespawnManager.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include <Scyther/Player/ScytherPlayerController.h>
#include <Scyther/ScytherGameModeBase.h>

// Sets default values
ASpawnPointPlayer::ASpawnPointPlayer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnPointCollider = CreateDefaultSubobject<UBoxComponent>( TEXT( "BoxCollider" ));
	SetRootComponent( SpawnPointCollider );
	CurrentCollider = FTransform(FQuat(0,0,0,0),FVector(0,0,0),FVector(0,0,0));
}

// Called when the game starts or when spawned
void ASpawnPointPlayer::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASpawnPointPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	OverlapSpawnPoint();
}

void ASpawnPointPlayer::OverlapSpawnPoint()
{
	TArray<UPrimitiveComponent*> overlapComps;
	SpawnPointCollider->GetOverlappingComponents( overlapComps );


	for( UPrimitiveComponent* comp : overlapComps )
	{
		if( comp->ComponentHasTag( "PlayerBody" ) )
		{
			AScytherPlayerController* playerController = Cast<AScytherPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0));
			if( IsValid( playerController ) )
			{
				ARespawnManager* Manager = Cast<AScytherGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))->respawnMan;
					
				FTransform CurrentSpawnPoint = Manager->GetSpawnPoint();
				
				if( CurrentSpawnPoint.GetLocation() != CurrentCollider.GetLocation() && CurrentSpawnPoint.GetRotation() != CurrentCollider.GetRotation() && CurrentSpawnPoint.GetScale3D() != CurrentCollider.GetScale3D() )
				{
					CurrentCollider = CurrentSpawnPoint;
					Manager->PushSpawnPoint( SpawnPointCollider->GetComponentTransform() );
					Manager->PopSpawnPoint();//en la propia funcion esta puesto para que solo borre si hay mas de un spawn point
				}
			}
		}
	}
}

