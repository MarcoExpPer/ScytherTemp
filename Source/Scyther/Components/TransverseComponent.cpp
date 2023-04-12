// Fill out your copyright notice in the Description page of Project Settings.

#include "TransverseComponent.h"
#include "../Items/SpawnPointPlayer.h"
#include "UObject/UObjectHash.h"
#include "Kismet/GameplayStatics.h"
#include "Scyther/Components/HealthComponent.h"
#include "../Player/ScytherPlayerPawn.h"


// Sets default values for this component's properties
UTransverseComponent::UTransverseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}


// Called when the game starts
void UTransverseComponent::BeginPlay()
{
	Super::BeginPlay();
	totalSpawnPoints = GotAllSpawnPoints().Num()-1;
	currentSpawnPoint = 0;
}


// Called every frame
void UTransverseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTransverseComponent::MoveToTheNextPos(FTransform pos ) 
{
	//int index = 0;
	if( totalSpawnPoints > 0 )
	{
		if(pos.GetLocation() != spawnPoints[currentSpawnPoint]->GetTransform().GetLocation() )
		{
			if(currentSpawnPoint<=totalSpawnPoints)
			{
				currentSpawnPoint++;
				UE_LOG( LogTemp, Display, TEXT( "currentSpawnPoint: %u" ), currentSpawnPoint );
				if( currentSpawnPoint == totalSpawnPoints)
				{
					currentSpawnPoint = 0;
				}
				ResetSpawnPoint( currentSpawnPoint );
			}
		}
	}
}

void UTransverseComponent::MoveToThePreviousPos( FTransform pos )
{
	if( totalSpawnPoints > 0 )
	{
		if( pos.GetLocation() != spawnPoints[currentSpawnPoint]->GetTransform().GetLocation() )
		{
			if( currentSpawnPoint <= totalSpawnPoints)
			{
				if( currentSpawnPoint == 0 )
				{
					currentSpawnPoint = totalSpawnPoints;
				}
				else if( currentSpawnPoint > 0 )
				{
					currentSpawnPoint--;
					UE_LOG( LogTemp, Display, TEXT( "CurrentSpawnPoint > 0 : %u" ), currentSpawnPoint );
				}
				UE_LOG( LogTemp, Display, TEXT( "Previous : %u" ), currentSpawnPoint );
				ResetSpawnPoint( currentSpawnPoint );
			}
			else
			{
				UE_LOG( LogTemp, Display, TEXT( "Reached at last Spawn Points: %u" ), currentSpawnPoint );
			}
		}
	}
}


void UTransverseComponent::ResetSpawnPoint(int _index)
{
	AScytherPlayerPawn* player = Cast<AScytherPlayerPawn>( UGameplayStatics::GetPlayerPawn( GetWorld(), 0 ) );

	player->healthComponent->changeHealth( 100.f, ElementTypes::NEUTRAL );

	const FVector LocationSpawn = spawnPoints[_index]->GetTransform().GetLocation();
	FHitResult HitResult;
	player->K2_SetActorLocation( LocationSpawn, false, HitResult, false );
}

TArray<AActor*> UTransverseComponent::GotAllSpawnPoints()
{
	//Get a reference to the UWorld.
	const UWorld* MyWorld = GetWorld();
	// change UObject to the type of UObject you're after
	for( TObjectIterator<ASpawnPointPlayer> ObjectItr; ObjectItr; ++ObjectItr )
	{
		// skip if this object is not associated with our current game world if that's not available
		if( ObjectItr->GetWorld() != MyWorld )
		{
			continue;
		}
		spawnPoints.Add( *ObjectItr );
	}
	return spawnPoints;
}