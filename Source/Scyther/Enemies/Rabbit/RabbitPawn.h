// Fill out your copyright notice in the Description page of Project Settings.

/*
Autor: Marco Expósito Pérez.

Last Update: 11/02/2023

File Overview:
Pawn del enemigo conejo. Es un enemigo sencillo que unicamente se mueve saltando hacia el objetivo y le ataca al estar cerca. En modo luz huye del jugador.
----------
PARAMETROS
----------
[ATTACK]
- maxJumpTimer: (float) El tiempo que debe ocurrir para que el conejo haga un salto.
- currentTimer: (float) El tiempo actual para que el conejo salte. Cuando llega a maxJumpTimer, salta.

----------
FUNCIONES
----------

*/

#pragma once

#include "fmod.hpp"
#include "FMODEvent.h"
#include "FMODBlueprintStatics.h"
#include <FMODAudioComponent.h>
#include "CoreMinimal.h"
#include "../BaseEnemy.h"
#include "../../Components/TargetMarkComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/BoxComponent.h"
#include "RabbitPawn.generated.h"


/**
 * 
 */
UCLASS()
class SCYTHER_API ARabbitPawn : public ABaseEnemy
{
	GENERATED_BODY()
	//Variables
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Conejo | Components", meta = ( DisplayName = "rabbit Body", ToolTip = "Cuerpo del conejo." ) )
		UStaticMeshComponent* rabbitBody;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Conejo | Components")
		UBoxComponent* rabbitHitBox;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Conejo | Components" )
		UBoxComponent* rabbitAttackArea;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Conejo | Components", meta = ( DisplayName = "movement Component" ) )
		UFloatingPawnMovement* movementComp;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Conejo | Blackboard Values", meta = ( DisplayName = "distance to detect player" ) )
		float distanceToDetectPlayer = 700;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Conejo | Blackboard Values", meta = ( DisplayName = "distance to attack player" ) )
		float distanceToAttackPlayer = 250;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Conejo | Audio", meta = ( DisplayName = "Sound Rabbit Attack",
		ToolTip = "Sound Rabbit Attack" ) )
		UFMODEvent* RabbitAttack;

private:
	FTimerHandle attackTimerHandle;
public:
	ARabbitPawn();

	virtual void BeginPlay() override;

	UFUNCTION( BlueprintCallable, meta = ( DisplayName = "Conejo | Attack" ) )
	void Attack();

	void CheckDamageArea();

	UFUNCTION( BlueprintCallable, meta = ( DisplayName = "Conejo | Reset attack" ) )
	void ResetAttack();

	UFUNCTION( BlueprintCallable, meta = ( DisplayName = "Conejo | Jump to position" ) )
	void JumpToPosition( FVector targetPosition );


};
