// Fill out your copyright notice in the Description page of Project Settings.


#include "LootableItem.h"
#include <Components/SphereComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Scyther/Player/ScytherPlayerPawn.h>
#include <Scyther/Components/EntityShadow.h>
#include <Kismet/GameplayStatics.h>
#include "GameFramework/RotatingMovementComponent.h"
#include <Scyther/ScytherGameModeBase.h>
#include <Scyther/Player/RespawnManager.h>

// Sets default values
ALootableItem::ALootableItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sphere = CreateDefaultSubobject<USphereComponent>( TEXT( "Collision Sphere" ) );
	SetRootComponent( sphere );


	mesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "Mesh" ) );
	mesh->SetupAttachment( sphere );
	mesh->SetCanEverAffectNavigation( false );

	rotComp = CreateDefaultSubobject<URotatingMovementComponent >( TEXT( "Rot Component" ) );

	entityShadow = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "EntityShadow" ) );
	entityShadow->SetupAttachment( RootComponent );
	entityShadow->SetCollisionEnabled( ECollisionEnabled::NoCollision );

	entityShadowComponent = CreateDefaultSubobject<UEntityShadow>( TEXT( "Entity Shadow Component" ) );
	AddOwnedComponent( entityShadowComponent );
}

// Called when the game starts or when spawned
void ALootableItem::BeginPlay()
{
	Super::BeginPlay();

	sphere->OnComponentBeginOverlap.AddDynamic( this, &ALootableItem::OnSphereOverlap );
	mesh->OnComponentBeginOverlap.AddDynamic( this, &ALootableItem::OnMeshOverlap );

	//Delay to fix bug of some of the world lottables not working
	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(
		   UnusedHandle, this, &ALootableItem::activateLootable, 2.f, false );

}

// Called every frame
void ALootableItem::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if( isFlyingTowardsPlayer )
	{
		currentTimer += DeltaTime;

		SetActorLocation( FMath::Lerp<FVector>(
			GetActorLocation(),
			UGameplayStatics::GetPlayerPawn( GetWorld(), 0 )->GetActorLocation(),
			currentTimer / timeToGetToPlayer ) );
	}
}

void ALootableItem::OnMeshOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	if( isReady )
		ComponentOverlapWithMesh( OtherComp );
}


void ALootableItem::playerMeshOverlap( AScytherPlayerPawn* player )
{
	GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, FString::Printf( TEXT( "--- PLAYER OVERLAP MESH NOT OVERRIDED---" ) ) );
}

void ALootableItem::OnSphereOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	if( isReady )
		ComponentOverlapWithSphere( OtherComp );
}

void ALootableItem::Respawn_Implementation()
{
	SetActorTransform( initialTransform );
	SetActorEnableCollision( true );
	SetActorHiddenInGame( false );
	entityShadow->SetVisibility( true );

	isFlyingTowardsPlayer = false;
	currentTimer = 0;
	isReady = true;

}

void ALootableItem::ComponentOverlapWithMesh( UPrimitiveComponent* comp )
{
	if( comp->ComponentHasTag( "PlayerBody" ) )
	{
		AScytherPlayerPawn* player = Cast<AScytherPlayerPawn>( comp->GetOwner() );
		if( player != nullptr )
		{
			playerMeshOverlap( player );
			isFlyingTowardsPlayer = false;

			if( LootableSound )
			{
				UFMODBlueprintStatics::PlayEvent2D( GetWorld(), LootableSound,  true );
			}

			if( doUIAnimation )
			{
				LootedEvent.Broadcast();
			}
			else
			{
				SetActorHiddenInGame( true );
			}

			entityShadow->SetVisibility( false );
			SetActorEnableCollision( false );

			if( isFromWorld )
			{
				respawnMan->AddToRespawnUntilCheckpointList( this );
				
			}
			else
			{
				if( !doUIAnimation )
				{
					Destroy();
				}
			}
		}
	}
}

void ALootableItem::ComponentOverlapWithSphere( UPrimitiveComponent* comp )
{
	if( comp->ComponentHasTag( "PlayerBody" ) )
	{
		currentTimer = 0;
		isFlyingTowardsPlayer = true;
	}
}


void ALootableItem::activateLootable()
{
	initialTransform = GetTransform();

	if( respawnMan == nullptr )
	{
		respawnMan = ( Cast<AScytherGameModeBase>( UGameplayStatics::GetGameMode( GetWorld() ) ) )->respawnMan;

		if( !isFromWorld )
		{
			respawnMan->playerDeadEvent.AddDynamic( this, &ALootableItem::OnPlayerRespawn );
		}
	}
	
	if( !isReady )
	{
		isReady = true;
		TArray<UPrimitiveComponent*> comps;

		mesh->GetOverlappingComponents( comps );
		for( UPrimitiveComponent* comp : comps )
		{
			ComponentOverlapWithMesh( comp );
		}

		sphere->GetOverlappingComponents( comps );
		for( UPrimitiveComponent* comp : comps )
		{
			ComponentOverlapWithSphere( comp );
		}

		isReady = true;


		rotComp->RotationRate = rotationRate;
		
	}
}

void ALootableItem::SetIsFromWorld( bool newIsFromWorld )
{
	isFromWorld = newIsFromWorld;
}

void ALootableItem::OnPlayerRespawn()
{
	Destroy();
}