// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDestructible.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Scyther/TargetingNet.h"
#include "Scyther/Components/DropLooteablesComponent.h"
#include "Scyther/Components/HealthComponent.h"

// Sets default values
AItemDestructible::AItemDestructible()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>( TEXT( "Scene Component" ) );


	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>( TEXT( "Destructible Component" ) );
	DestructibleComponent->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform, "" );
	DestructibleComponent->SetNotifyRigidBodyCollision( true );

	TriggerComponent = CreateDefaultSubobject<UBoxComponent>( TEXT( "Trigger Component" ) );
	TriggerComponent->AttachToComponent( DestructibleComponent, FAttachmentTransformRules::KeepRelativeTransform, "" );

	DetectGravity = CreateDefaultSubobject<UBoxComponent>( TEXT( "Detect objectos to enable gravity" ) );
	DetectGravity->AttachToComponent( DestructibleComponent, FAttachmentTransformRules::KeepRelativeTransform, "" );

	healthComp = CreateDefaultSubobject<UHealthComponent>( TEXT( "Health" ) );
	LooteableComp = CreateDefaultSubobject<UDropLooteablesComponent>( TEXT( "Drop Looteables" ) );

	FMODAudio = CreateDefaultSubobject<UFMODAudioComponent>( TEXT( "FMOD Audio" ) );
	FMODAudio->AttachToComponent( DestructibleComponent, FAttachmentTransformRules::KeepRelativeTransform, "" );
}

void AItemDestructible::DisabledGravity( UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	if( HasPhysics == true )
	{
		if( OtherActor != this && OtherActor != UGameplayStatics::GetPlayerPawn( GetWorld(), 0 ) && OtherActor->GetClass() != ATargetingNet::StaticClass() && OtherActor != nullptr )
		{
			DestructibleComponent->SetSimulatePhysics( false );
			TriggerComponent->SetSimulatePhysics( false );
			DetectGravity->SetSimulatePhysics( false );
			DestructibleComponent->SetEnableGravity( false );
			TriggerComponent->SetEnableGravity( false );
			DetectGravity->SetEnableGravity( false );
			DestructibleComponent->ComponentVelocity.Set( 0.f, 0.f, 0.f );
			TriggerComponent->ComponentVelocity.Set( 0.f, 0.f, 0.f );
			DetectGravity->ComponentVelocity.Set( 0.f, 0.f, 0.f );
			//GEngine->AddOnScreenDebugMessage( -1, 15.f, FColor::Orange, FString::Printf(TEXT("OtherActor colisionando: %F"), OtherActor->GetName() ));
			/*DestructibleComponent->SetEnableGravity( false );
			DestructibleComponent->ComponentVelocity = FVector( 0.f, 0.f, 0.f );*/
		}
	}
	else
	{
		DestructibleComponent->SetSimulatePhysics( false );
		TriggerComponent->SetSimulatePhysics( false );
		DetectGravity->SetSimulatePhysics( false );
		DestructibleComponent->SetEnableGravity( false );
		TriggerComponent->SetEnableGravity( false );
		DetectGravity->SetEnableGravity( false );
	}
}

void AItemDestructible::DisabledGravityHit( UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit )
{
	if( HasPhysics == true )
	{
		if( OtherActor != UGameplayStatics::GetPlayerPawn( GetWorld(), 0 ) && OtherActor != nullptr )
		{
			DestructibleComponent->SetSimulatePhysics( false );
			TriggerComponent->SetSimulatePhysics( false );
			DetectGravity->SetSimulatePhysics( false );
			DestructibleComponent->SetEnableGravity( false );
			TriggerComponent->SetEnableGravity( false );
			DetectGravity->SetEnableGravity( false );
			DestructibleComponent->ComponentVelocity.Set( 0.f, 0.f, 0.f );
			TriggerComponent->ComponentVelocity.Set( 0.f, 0.f, 0.f );
			DetectGravity->ComponentVelocity.Set( 0.f, 0.f, 0.f );

		}
	}
	else
	{
		DestructibleComponent->SetSimulatePhysics( false );
		TriggerComponent->SetSimulatePhysics( false );
		DetectGravity->SetSimulatePhysics( false );
		DestructibleComponent->SetEnableGravity( false );
		TriggerComponent->SetEnableGravity( false );
		DetectGravity->SetEnableGravity( false );
	}
}

void AItemDestructible::EnabledGravity( UPrimitiveComponent* OverlappedComponent,
										AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	if( HasPhysics == true )
	{
		if( OtherActor != this && OtherActor != UGameplayStatics::GetPlayerPawn( GetWorld(), 0 ) && OtherActor->GetClass() != ATargetingNet::StaticClass() && OtherActor != nullptr )
		{
			DestructibleComponent->SetSimulatePhysics( true );
			TriggerComponent->SetSimulatePhysics( true );
			DetectGravity->SetSimulatePhysics( true );
			DestructibleComponent->SetEnableGravity( true );
			TriggerComponent->SetEnableGravity( true );
			DetectGravity->SetEnableGravity( true );

		}
	}
	else
	{
		DestructibleComponent->SetSimulatePhysics( false );
		TriggerComponent->SetSimulatePhysics( false );
		DetectGravity->SetSimulatePhysics( false );
		DestructibleComponent->SetEnableGravity( false );
		TriggerComponent->SetEnableGravity( false );
		DetectGravity->SetEnableGravity( false );
	}
}


// Called when the game starts or when spawned
void AItemDestructible::BeginPlay()
{
	Super::BeginPlay();

	if( HasPhysics == true )
	{
		DetectGravity->OnComponentBeginOverlap.AddDynamic( this, &AItemDestructible::DisabledGravity );
		DetectGravity->OnComponentEndOverlap.AddDynamic( this, &AItemDestructible::EnabledGravity );
		DetectGravity->OnComponentHit.AddDynamic( this, &AItemDestructible::DisabledGravityHit );
	}
	else
	{
		DestructibleComponent->SetSimulatePhysics( false );
		TriggerComponent->SetSimulatePhysics( false );
		DetectGravity->SetSimulatePhysics( false );
		DestructibleComponent->SetEnableGravity( false );
		TriggerComponent->SetEnableGravity( false );
		DetectGravity->SetEnableGravity( false );
	}
	TriggerComponent->OnComponentBeginOverlap.AddDynamic( this, &AItemDestructible::Trigger );

	healthComp->zerohealthEvent.AddDynamic( this, &AItemDestructible::Destroy );
}

// Called every frame
void AItemDestructible::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
} 

void AItemDestructible::Trigger( UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult )
{


	if( !IsDestroyed && OtherComp->ComponentHasTag( "ScytheEdgeCollision" ) )
	{
		GEngine->AddOnScreenDebugMessage( -1, 3.f, FColor::Orange, __FUNCTION__ );
		if( CanDestroy == true )
		{
			hitsReceived++;
			//sonido de cuando recibe un golpe cuando es destruible
			DoHitSound.Broadcast();
			if( hitsReceived >= HitsToDestroy )
			{
				DestructibleComponent->SetSimulatePhysics( true );
				DestructibleComponent->SetEnableGravity( true );
				HitLocation = DestructibleComponent->GetComponentLocation();
				ImpulseDir = DestructibleComponent->GetForwardVector();
				healthComp->receiveDamage( healthComp->maxHp, DamageModes::INEVITABLE, true );
			}
		}
		else
		{
			DoHitSound.Broadcast();
			//sonido de que es indestructible
		}

	}

}


void AItemDestructible::Destroy( DamageModes damageType )
{
	GEngine->AddOnScreenDebugMessage( -1, 3.f, FColor::Orange, __FUNCTION__ );
	
	DestructibleComponent->ApplyDamage( DefaultDamage, HitLocation, FVector( 0.f, 0.f, -100.f ), ImpulseStrength );
	DestroySound.Broadcast();
	LooteableComp->DropLooteables( TriggerComponent->GetComponentLocation(), true );
	TriggerComponent->DestroyComponent();
	DetectGravity->DestroyComponent();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer( TimerHandle, this, &AItemDestructible::DeSpawn, 2.0f, false );
}

void AItemDestructible::DeSpawn()
{
	DestructibleComponent->DestroyComponent();

}