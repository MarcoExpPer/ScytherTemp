// Fill out your copyright notice in the Description page of Project Settings.

#include "BreakableFloor.h"
#include <Kismet/GameplayStatics.h>
#include <Scyther/Components/HealthComponent.h>
#include "GameFramework/Actor.h"
#include <DrawDebugHelpers.h>
#include "TimerManager.h"

// Sets default values
ABreakableFloor::ABreakableFloor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BreakableFloorCollision = CreateDefaultSubobject<UBoxComponent>( TEXT ("BreakableFloorCollision") );
	SetRootComponent( BreakableFloorCollision );

	//Box Collision

	BreakableFloor = CreateDefaultSubobject<UStaticMeshComponent>( TEXT ("BreakableFloor") );
	BreakableFloor->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform, "" );

	/// <summary>
	/// Set the values that each variable have
	/// </summary>

	FallDistance.Set( 0.0f, 0.0f, 0.5f );
	PlatformRotation = FRotator( 0, 0, 5 );
	


}

// Called when the game starts or when spawned
void ABreakableFloor::BeginPlay()
{
	Super::BeginPlay();

	//UHealthComponent* health = Cast<UHealthComponent>( UGameplayStatics::GetPlayerPawn( GetWorld(), 0 )
	//	->GetComponentByClass( UHealthComponent::StaticClass() ) );

	//health->zerohealthEvent.AddDynamic( this, &ABreakableFloor::PlayerDeath );

	//LLama a la funcion del onOverlapEnter cuando empieza a colisionar

	BreakableFloorCollision->OnComponentBeginOverlap.AddDynamic( this, &ABreakableFloor::BeginCollision );

	//

	originalPlatform = GetTransform();

}

// Called every frame
void ABreakableFloor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ABreakableFloor::BeginCollision( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{

	if( OtherComp->ComponentHasTag( "PlayerBody" ) && isDespawning != true )
	{
		/// <summary>
		/// When the box detect the collision with the player, it fall down (axis Z) a distance that is set in the variable FallDistance
		/// </summary>

		isDespawning = true;
		FVector originalPlatformPosition = GetActorLocation();
		FVector NewPlatformPosition = ( -FallDistance ) + originalPlatformPosition;

		//Sounds of cracking wood

		InstanceWraper = UFMODBlueprintStatics::PlayEventAtLocation( GetWorld(), EventCrackingWood, GetActorTransform(), true );


		bool SetActorLocation( const FVector & NewPlatformPosition, bool bSweep, FHitResult * OutSweepHitResult, ETeleportType Teleport );

		if( PlatformDurationRotationTimerHandle.IsValid() )
		{
			PlatformDurationRotationTimerHandle.Invalidate();
		}
		GetWorldTimerManager().SetTimer( PlatformDurationRotationTimerHandle, this, &ABreakableFloor::RotatePlatform, PlatformDurationRotation, false );
	}

}

void ABreakableFloor::RotatePlatform()
{
	//La plataforma rota un poco antes de romperse

	FRotator originalPlatformRotation = GetActorRotation();
	FRotator NewPlatformRotation = PlatformRotation + originalPlatformRotation;
	SetActorRotation( FRotator( NewPlatformRotation ), ETeleportType::None );

	//Delay para la destrucción de la plataforma

	if( PlatformDestructionDurationTimerHandle.IsValid() )
	{
		PlatformDestructionDurationTimerHandle.Invalidate();
	}

	GetWorldTimerManager().SetTimer( PlatformDestructionDurationTimerHandle, this, &ABreakableFloor::HidePlatform, PlatformDurationDestruction, false );
}

void ABreakableFloor::HidePlatform()
{
	//La plataforma desaparece y no puedes verla

	BreakableFloor->SetVisibility( false, true );

	//Sonido Romper la Madera

	InstanceWraper = UFMODBlueprintStatics::PlayEventAtLocation(GetWorld(), EventBreakingWood, GetActorTransform(), true);
	

	//La plataforma desaparece y no puedes colisionar con ella

	BreakableFloor->SetCollisionEnabled( ECollisionEnabled::NoCollision );

	if( PlatformRespawnTimerHandle.IsValid() )
	{
		PlatformRespawnTimerHandle.Invalidate();
	}

	GetWorldTimerManager().SetTimer( PlatformRespawnTimerHandle, this, &ABreakableFloor::RespawnPlatform, PlatformRespawn, false );

}
/*
void ABreakableFloor::PlayerDeath( ElementTypes type )
{
	RespawnPlatform();
}*/

void ABreakableFloor::RespawnPlatform()
{
	if( Respawn == true)
	{
		isDespawning = false;
		FHitResult* OutSweepHitResult = false;
		SetActorTransform( originalPlatform, false, OutSweepHitResult, ETeleportType::None );

		//Plataforma respawnea y puedes verla

		BreakableFloor->SetVisibility( true, true );

		//La plataforma aparece y puedes colisionar con ella

		BreakableFloor->SetCollisionEnabled( ECollisionEnabled::QueryAndPhysics );
	}

}
