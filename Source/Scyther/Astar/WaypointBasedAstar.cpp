// Fill out your copyright notice in the Description page of Project Settings.


#include "WaypointBasedAstar.h"

// Sets default values for this component's properties
UWaypointBasedAstar::UWaypointBasedAstar()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UWaypointBasedAstar::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UWaypointBasedAstar::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


AAstarWaypoint* UWaypointBasedAstar::CalculateTarget(FVector targetLocation)
{
	closedList.Empty();
	openList.Clear();

	currentWaypoint->fatherWP = nullptr;

	if (IsLocationInsideSphere(currentWaypoint->GetActorLocation().X, currentWaypoint->GetActorLocation().Y, currentWaypoint->GetActorLocation().Z,
		currentWaypoint->collision->GetScaledSphereRadius(), targetLocation))
	{
		return currentWaypoint;
	}

	TArray<AAstarWaypoint*> nbwps;
	TPriorityQueueNode<AAstarWaypoint*> wpToCheck;

	FVector currentLocation;
	bool hasSolution = false;

	AAstarWaypoint* solution = nullptr;

	
	currentWaypoint->G = 0;
	currentWaypoint->H = FVector::Distance(currentWaypoint->GetActorLocation(), targetLocation);

	openList.Push(currentWaypoint, currentWaypoint->G + currentWaypoint->H);
	
	int count = 0;
	while (!openList.IsEmpty() && !hasSolution && count < 20) {
		count++;
		wpToCheck = openList.PopNode();
		closedList.Add(wpToCheck.Element);

		nbwps = wpToCheck.Element->connectedWayPoints;

		for (AAstarWaypoint* wp : nbwps) {
			if (!closedList.Contains(wp)) {
				if (openList.Contains(wp)) {

					float newG = wpToCheck.Element->G + FVector::Distance(wpToCheck.Element->GetActorLocation(), wp->GetActorLocation());

					if (wp->G > newG) {

						wp->G = newG;
						wp->fatherWP = wpToCheck.Element;

						openList.Push(wp, wp->G + wp->H);
					}
				}
				else {
					wp->fatherWP = wpToCheck.Element;
					wp->G = wp->fatherWP->G + FVector::Distance(wp->fatherWP->GetActorLocation(), wp->GetActorLocation());
					wp->H = FVector::Distance(wp->GetActorLocation(), targetLocation);

					openList.Push(wp, wp->G + wp->H);

					if (IsLocationInsideSphere(wp->GetActorLocation().X, wp->GetActorLocation().Y, wp->GetActorLocation().Z,
						wp->collision->GetScaledSphereRadius(), targetLocation))
					{
						hasSolution = true;
						solution = wp;
					}
				}
			}
		}
	}
	return solution;

	return currentWaypoint;
}

bool UWaypointBasedAstar::IsLocationInsideSphere(float cx, float cy, float cz, float radius, FVector location) {
	float xDistance = FMath::Pow(location.X - cx, 2);
	float yDistance = FMath::Pow(location.Y - cy, 2);
	float zDistance = FMath::Pow(location.Z - cz, 2);

	float realDistance = (xDistance + yDistance + zDistance);

	return (realDistance <= FMath::Pow(radius, 2));
}
