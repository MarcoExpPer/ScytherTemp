// Fill out your copyright notice in the Description page of Project Settings.


#include "Scythe.h"

// Sets default values
AScythe::AScythe()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Scythe mesh"));
	RootComponent = mesh;

	edgeCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Edge hitbox"));
	edgeCollision->SetupAttachment(RootComponent);

	handleCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Handle hitbox"));
	handleCollision->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AScythe::BeginPlay()
{
	Super::BeginPlay();

	baseEdgeCollisionLocation = edgeCollision->GetRelativeLocation();
	baseEdgeRadius = edgeCollision->GetUnscaledSphereRadius();
	rangeDirection.Normalize();
}

// Called every frame
void AScythe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AScythe::UpdateLength(float rangeMultiplier) {
	edgeCollision->SetRelativeLocation(baseEdgeCollisionLocation + (rangeDirection * (rangeMultiplier - 1) * 15));
	edgeCollision->SetSphereRadius(baseEdgeRadius + baseEdgeRadius * (rangeMultiplier - 1));
}


void AScythe::toggleHitBox(ECollisionEnabled::Type newCollision) {
	edgeCollision->SetCollisionEnabled(newCollision);
	handleCollision->SetCollisionEnabled(newCollision);
}