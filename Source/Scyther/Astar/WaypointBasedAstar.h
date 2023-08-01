// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AstarWaypoint.h"
#include "PriorityArray.h"
#include "Components/ActorComponent.h"
#include "WaypointBasedAstar.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SCYTHER_API UWaypointBasedAstar : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint", meta = (DisplayName = "Current Waypoint"))
		AAstarWaypoint* currentWaypoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Waypoint", meta = (DisplayName = "Target Waypoint"))
		AAstarWaypoint* targetWaypoint;

	TPriorityQueue<AAstarWaypoint*> openList;
	TArray<AAstarWaypoint*> closedList;

	// Sets default values for this component's properties
	UWaypointBasedAstar();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Waypoint Calculator", meta = (DisplayName = "Calculate Target"))
		AAstarWaypoint* CalculateTarget(FVector targetLocation);

	bool IsLocationInsideSphere(float cx, float cy, float cz, float radius, FVector location);
};
