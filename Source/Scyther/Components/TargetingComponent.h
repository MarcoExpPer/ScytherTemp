// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "Math/Vector.h"
#include "Kismet/KismetMathLibrary.h"
#include "../TargetingNet.h"
#include "../Interfaces/CanBeZtargeted.h"
#include <Scyther/Player/ScytherPlayerPawn.h>
#include "../Enemies/BaseEnemy.h"
#include "TargetMarkComponent.h"

#include "TargetingComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCYTHER_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTargetingComponent();

	//UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Targeting", meta = ( DisplayName = "Targeting Net (Class)" ) )
	UClass* targetNetClass = ATargetingNet::StaticClass();

	ATargetingNet* targetNet;

	AActor* playerTarget = nullptr;

private:

	AScytherPlayerPawn* character;
	TArray<AActor*> posibleTargets;

	TArray<AActor*> enemiesInVisionCone;
	TArray<AActor*> othersInVisionCone;
	TArray<AActor*> enemiesOutVisionCone;
	TArray<AActor*> othersOutVisionCone;

	FVector2D switchTargetInput = FVector2D( 0, 0 );
	int alreadySwitchedTarget = 0;	//Contador para el input del joystick
	bool targetSwitchedRight = false;
	bool targetSwitchedUp = false;

	int targetOutOffSight = 0;	//Contador para dar un poco de tiempo al quitar el fijado de un enemigo que ya no es visible

	UTargetMarkComponent* targetMark;
	TArray<AActor*> markedTargets;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//Returns a list of posible targets
	TArray<AActor*> GetPosibleTargets( float range );

	//Returns all the actors in range from the player
	TArray<AActor*> FindActorsInRange( float range );

	//Return Enemies and Other actors only. In one list joined or separated in two.
	TArray<AActor*> GetJoinedEnemiesAndOthers( TArray<AActor*>& list );
	void GetSeparatedEnemiesAndOthers( TArray<AActor*>& list, TArray<AActor*>& outEnemies, TArray<AActor*>& outOthers );

	//Returns the closest actor to the player or the target from a list
	AActor* GetClosest( TArray<AActor*>& list );
	AActor* GetClosestToTarget( TArray<AActor*>& list );

	//Returns a list with only the actors that are visible for the player
	TArray<AActor*> GetVisible( TArray<AActor*>& list );

	//Returns if an actor is visible or not by the player
	bool IsVisible( AActor* target );

	//Fill the list of actors and others with the content pass by parameter
	void ClassifyPosibleTargets();
	
	//Returns the target based on the priorities
	AActor* ChooseTarget();

	//Switch the current target to another actor in the direction
	void SwitchTarget();

	//Activate the posible objetive mark on all actors from a list
	void MarkTargets();

public:
	//Get and set for Target
	AActor* GetTarget();
	void SetTarget( AActor* target );
	// Get for possible targets
	TArray<AActor*> GetTargetsByRange(float range);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION( BlueprintCallable, meta = ( DisplayName = "Find Target" ) )
	void FindTarget();

	void SwitchTargetRight( float value );
	void SwitchTargetUp( float alue );	
};
