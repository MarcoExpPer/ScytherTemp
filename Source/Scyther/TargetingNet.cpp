// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingNet.h"

#include "Components/SphereComponent.h"


USphereComponent* sphereCollision;

// Sets default values
ATargetingNet::ATargetingNet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sphereCollision = CreateDefaultSubobject<USphereComponent>( TEXT( "TargetCollision" ));
	SetRootComponent( sphereCollision );
	SetCollisionRadius( 500 );
}

// Called when the game starts or when spawned
void ATargetingNet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATargetingNet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ATargetingNet::SetCollisionRadius( float radius )
{
	if( sphereCollision )
	{
		sphereCollision->SetSphereRadius( radius );
	}
	else
	{
		UE_LOG( LogTemp, Error, TEXT( "Error: No esta instanciada la esfera de colisiones." ) );
	}
}
