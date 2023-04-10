// Fill out your copyright notice in the Description page of Project Settings.


#include "ManaDrop.h"
#include <Scyther/Player/ScytherPlayerPawn.h>
#include <Scyther/Components/SkillComponent.h>
#include <Kismet/GameplayStatics.h>

// Sets default values
AManaDrop::AManaDrop()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "Mana mesh" ) );
	SetRootComponent( mesh );

	sphere = CreateDefaultSubobject<USphereComponent>( TEXT( "Sphere collision" ) );
	sphere->SetupAttachment( RootComponent );

	mesh->OnComponentBeginOverlap.AddDynamic( this, &AManaDrop::OnMeshOverlap );
	sphere->OnComponentBeginOverlap.AddDynamic( this, &AManaDrop::OnSphereOverlap );
}


void AManaDrop::BeginPlay()
{
	Super::BeginPlay();

	SetActorHiddenInGame( true );
	SetActorEnableCollision( false );
	SetActorTickEnabled( false );
}

void AManaDrop::OnMeshOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	if( OtherComp->ComponentHasTag( "PlayerBody" ))
	{
		AScytherPlayerPawn *player = Cast<AScytherPlayerPawn>( OtherActor );
		if( player != nullptr )
		{
			player->skillComponent->UpdateMana( isLightLoot, manaAmount);
			player->currentExperience += expAmount;

			deSpawn();
		}
	}
}

void AManaDrop::OnSphereOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	if( OtherComp->ComponentHasTag( "PlayerBody" ) )
	{
		StartFlying();
	}
}


void AManaDrop::deSpawn()
{
	SetActorHiddenInGame( true );
	SetActorEnableCollision( false );
	SetActorTickEnabled( false );

	isFlyingToPlayer = false;
}

void AManaDrop::spawn(bool newMode, float newMana, float newExp)
{
	SetActorHiddenInGame( false );
	SetActorEnableCollision( true );
	SetActorTickEnabled( true );

	this->isLightLoot = newMode;
	this->manaAmount = newMana;
	this->expAmount = newExp;
	

	mesh->SetMaterial( 0, this->isLightLoot ? lightMat : darkMat );
}

// Called every frame
void AManaDrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if( isFlyingToPlayer )
	{
		currentTimer += DeltaTime;
		SetActorLocation(FMath::Lerp<FVector>( GetActorLocation(), UGameplayStatics::GetPlayerPawn( GetWorld(), 0 )->GetActorLocation(), currentTimer/ timeToGetToPlayer ));
	}
}

void AManaDrop::StartFlying()
{
	currentTimer = 0;
	isFlyingToPlayer = true;
	
}