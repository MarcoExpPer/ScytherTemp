// Fill out your copyright notice in the Description page of Project Settings.

/*
Autor: Marco Expósito Pérez.

Last Update: 11/02/2023

File Overview:
Este AIControlador es un controlador basico que tiene la funcionalidad genérica de un controlador de un enemigo basico
----------
FUNCIONES ninguna esta implementada y deberian implementarse desde los hijos.
----------
//la idea de estas funciones es que cuando un enemigo muere, va a despawnear, y cuando el jugador respawnee o algo asi, 
- spawn. Spawnea al enemigo en una posición concreta.
- unSpawn. Despawnea al enemigo.

- DoAttack. Hace el ataque básico hacia una posicicon objetivo que tenga el enemigo, si es que lo tiene.
- moveTo. Mueve el enemigo a alguna posicion.
- die. Mata al enemigo y lo termina despawneando.

*/

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseEnemy.h"
#include "../Items/ManaDrop.h"

#include "BaseEnemyCtrl.generated.h"




UCLASS()
class SCYTHER_API ABaseEnemyCtrl : public AAIController
{
	GENERATED_BODY()
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Controller", meta = ( DisplayName = "EnemyBasePawn" ) )
	ABaseEnemy *enemyPawn;


	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Controller", meta = ( DisplayName = "EnemyBasePawn" ) )
	FVector lootOnDeadSpread;

public:
	virtual void BeginPlay() override;



	//Devuelve un booleano para poder indicar si el ataque se ha realizado o si por alguna razon, se ha tenido que cancelar.
	UFUNCTION( BlueprintCallable, Category = "BaseEnemyController", meta = ( DisplayName = "DoBasicAttack" ) )
	virtual bool DoBasicAttack( FVector targetPosition );
	UFUNCTION( BlueprintCallable, Category = "BaseEnemyController", meta = ( DisplayName = "MoveToPosition" ) )
	virtual void moveToPosition( FVector targetPosition );

	UFUNCTION( BlueprintCallable, Category = "BaseEnemyController", meta = ( DisplayName = "HitByAttack" ) )
	virtual void hitByAttack();
	UFUNCTION( BlueprintCallable, Category = "BaseEnemyController", meta = ( DisplayName = "Die" ) )
	virtual void die();

	UFUNCTION( BlueprintCallable, Category = "BaseEnemyController", meta = ( DisplayName = "Rotate towards target" ) )
	void rotatePawnTowardsTargetXY( FVector targetPosition );
};
