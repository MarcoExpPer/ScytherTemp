// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PriorityArray.h"
#include "Components/SphereComponent.h"
#include "AstarWaypoint.generated.h"

UCLASS()
class SCYTHER_API AAstarWaypoint : public AActor
{
	GENERATED_BODY()
	
public:	


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint" , meta = (DisplayName = "Collision Sphere"))
		USphereComponent* collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Waypoint", meta = (DisplayName = "Connected Waypoints"))
		TArray<AAstarWaypoint*> connectedWayPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Waypoint", meta = (DisplayName = "Parent WP"))
		AAstarWaypoint *fatherWP;

	float G;
	float H;

	// Sets default values for this actor's properties
	AAstarWaypoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



private:
	void CheckNearbyWaypoints();

};
