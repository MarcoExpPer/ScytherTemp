// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Player/ScytherPlayerPawn.h"
#include "Components/StaticMeshComponent.h"
#include "EntityShadow.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCYTHER_API UEntityShadow : public UActorComponent
{
	GENERATED_BODY()

private:

	/// The character
	AScytherPlayerPawn* character = nullptr;

	/// The lootable
	class ALootableItem* lootableItem = nullptr;
	
	/// The player shadow
	UStaticMeshComponent* entityShadow = nullptr;	
	
	/// The player circle ring
	UStaticMeshComponent* circleRingShadow = nullptr;


public:	
	// Sets default values for this component's properties
	UEntityShadow();

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		bool doTick = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void TPtoFloor();
};
