// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "fmod.hpp"
#include "FMODEvent.h"
#include "FMODBlueprintStatics.h"
#include <FMODAudioComponent.h>
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "Math/Vector.h"
#include "Kismet/KismetMathLibrary.h"
#include "../TargetingNet.h"
#include "../Interfaces/CanBeZtargeted.h"
#include <Scyther/Player/ScytherPlayerPawn.h>
#include "../Enemies/BaseEnemy.h"
#include "../Enemies/Worm/WormPawn.h"
#include "TargetMarkComponent.h"

#include "TargetingComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCYTHER_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTargetingComponent();

	ATargetingNet* targetNet;

	AActor* playerTarget = nullptr;

	AActor* actualClosestEnemy = nullptr;

	TArray<AActor*> enemiesInVisionCone;
	TArray<AActor*> othersInVisionCone;
	TArray<AActor*> enemiesOutVisionCone;
	TArray<AActor*> othersOutVisionCone;
private:

	AScytherPlayerPawn* character;
	TArray<AActor*> posibleTargets;



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

	AActor* GetClosestToTarget( TArray<AActor*>& list );

	//Returns a list with only the actors that are visible for the player
	TArray<AActor*> GetVisible( TArray<AActor*>& list );

	//Returns if an actor is visible or not by the player
	bool IsVisible( AActor* target );
	
	//Returns the target based on the priorities
	AActor* ChooseTarget();

	//Switch the current target to another actor in the direction
	void SwitchTarget();

	//Activate the posible objetive mark on all actors from a list
	void MarkTargets();

	//---Targeting---//
public:
	AActor* GetClosest( TArray<AActor*>& list );

	//Fill the list of actors and others with the content pass by parameter
	void ClassifyPosibleTargets();

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Targeting", meta = ( DisplayName = "Target Mode" ) )
		/// Returns whether the current Z-targeting mode is in NONE, SIMPLE or ADVANCED.
		TEnumAsByte<ZTargetModes> targetMode = ZT_Simple;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Targeting", meta = ( DisplayName = "Target Range" ) )
		/// The max range a selected enemy can ever be in the moment of choosing.
		float targetRange = 1000;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Targeting", meta = ( DisplayName = "Maximun Target Distance" ) )
		/// The max distance a selected enemy can ever be before automatically returning to normal mode.
		float maxTargetDistance = 2000;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Targeting", meta = ( DisplayName = "Player Vision" ) )
		/// The angle of vision of the character that defines the surface where enemies can be targeted.
		FVector2D playerVisionAngle = ( 120, 80 );
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Targeting", meta = ( DisplayName = "Only Target in Vision Cone" ) )
		/// Whether or not you want the player to only target the actors that are inside the vision cone of the player.
		bool onlyTargetVisionCone = false;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Targeting", meta = ( DisplayName = "Needs Vision" ) )
		/// Whether or not bbjetives need to be on sight in order to be targeted.
		bool needsVision = true;

	//------------------------//
	//   FMOD Target Sound
	//------------------------//

	UPROPERTY( EditAnywhere, Category = "Audio | FMOD", meta = ( DisplayName = "Z-Target Sound Activated",
		ToolTip = "Sonido de cuando el enemigo ha sido fijado por el Z-Target" ) )
		/// <summary><value>Z-Target sound when is activated.
		UFMODEvent* ZTargetset;
	UPROPERTY( EditAnywhere, Category = "Audio | FMOD", meta = ( DisplayName = "Z-Target Sound Defuse",
		ToolTip = "Sonido de cuando el enemigo ha sido desfijado por el Z-Target" ) )
		/// <summary><value>Z-Target sound when is desactivated.
		UFMODEvent* ZTargetdefuse;


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
