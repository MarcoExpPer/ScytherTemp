// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TransverseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCYTHER_API UTransverseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTransverseComponent();
	
	void MoveToTheNextPos( FTransform pos );
	void MoveToThePreviousPos( FTransform pos );

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY( EditAnywhere )
		TArray<AActor*> spawnPoints;

	void ResetSpawnPoint(int _index);
	TArray<AActor*> GotAllSpawnPoints();
private:
	int currentSpawnPoint;
	int totalSpawnPoints;
	int index;
};
