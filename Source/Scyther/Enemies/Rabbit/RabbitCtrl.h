// Fill out your copyright notice in the Description page of Project Settings.

/*
Autor: Marco Expósito Pérez.

Last Update: 11/02/2023

File Overview:
Controlador del enemigo conejo. Es un enemigo sencillo que unicamente se mueve saltando hacia el objetivo y le ataca al estar cerca. En modo luz huye del jugador.
----------
PARAMETROS
----------
[JUMP]
- maxJumpTimer: (float) El tiempo que debe ocurrir para que el conejo haga un salto.
- currentTimer: (float) El tiempo actual para que el conejo salte. Cuando llega a maxJumpTimer, salta.

*/

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RabbitPawn.h"
#include "../BaseEnemyCtrl.h"
#include "RabbitCtrl.generated.h"

UCLASS()
class SCYTHER_API ARabbitCtrl : public ABaseEnemyCtrl
{
	GENERATED_BODY()

public:
//Variables
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Conejo | Controller", meta = ( DisplayName = "rabbit pawn", ToolTip = "Pawn del conejo." ) )
	ARabbitPawn *rabbitPawn;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Conejo | Editar en el ConejoPawn", meta = ( DisplayName = "distance to detect player") )
		float distanceToDetectPlayer;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Conejo | Editar en el ConejoPawn", meta = ( DisplayName = "distance to attack player" ) )
		float distanceToAttackPlayer;

//Functions
public:

	ARabbitCtrl();

	virtual void BeginPlay() override;

	void Tick( float DeltaTime );

	virtual EPathFollowingRequestResult::Type moveToPosition( FVector targetPosition );
	virtual bool DoBasicAttack( FVector targetPosition );


};
