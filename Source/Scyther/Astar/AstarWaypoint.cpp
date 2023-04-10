// Fill out your copyright notice in the Description page of Project Settings.


#include "AstarWaypoint.h"

// Sets default values
AAstarWaypoint::AAstarWaypoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere component"));
	RootComponent = collision;

}

// Called when the game starts or when spawned
void AAstarWaypoint::BeginPlay()
{
	Super::BeginPlay();

	//Necesary delay to detect all nearby overlaped components
	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(
		UnusedHandle, this, &AAstarWaypoint::CheckNearbyWaypoints, 0.5f, false);
}

void AAstarWaypoint::CheckNearbyWaypoints() {
	TArray<AActor*> output;
	collision->GetOverlappingActors(output, AAstarWaypoint::StaticClass());

	for (AActor* actor : output) {
		connectedWayPoints.Add(Cast<AAstarWaypoint>(actor));
	}


#if WITH_EDITOR 
	if (connectedWayPoints.Num() != 0) {
		collision->SetHiddenInGame(false);
	}
#endif
}

// Called every frame
void AAstarWaypoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

