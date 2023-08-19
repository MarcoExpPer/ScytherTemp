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
#include "BaseEnemyCtrl.generated.h"




UCLASS()
class SCYTHER_API ABaseEnemyCtrl : public AAIController
{
	GENERATED_BODY()
public:
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "BaseEnemy | Controller", meta = ( DisplayName = "EnemyBasePawn" ) )
	ABaseEnemy *enemyPawn;

	class AScytherGameModeBase* gm;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "BaseEnemy | Controller", meta = ( DisplayName = "EnemyBasePawn" ) )
	FVector lootOnDeadSpread;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		combatState currentCombatState = combatState::inCombat;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		int attacksToidle = 2;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		int currentAttacksLeftToidle = 0;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		int combatPoolSize = 1;

public:
	virtual void BeginPlay() override;

	//-- Family Combat --/


	//Devuelve un booleano para poder indicar si el ataque se ha realizado o si por alguna razon, se ha tenido que cancelar.
	UFUNCTION( BlueprintCallable, Category = "BaseEnemy | Controller", meta = ( DisplayName = "DoBasicAttack" ) )
	virtual bool DoBasicAttack( FVector targetPosition );
	UFUNCTION( BlueprintCallable, Category = "BaseEnemy | Controller", meta = ( DisplayName = "MoveToPosition" ) )
	virtual EPathFollowingRequestResult::Type moveToPosition( FVector targetPosition );

	UFUNCTION( BlueprintCallable, Category = "BaseEnemy | Controller", meta = ( DisplayName = "HitByAttack" ) )
	virtual void hitByAttack();
	UFUNCTION( BlueprintCallable, Category = "BaseEnemy | Controller", meta = ( DisplayName = "Die" ) )
	virtual void die();

	UFUNCTION( BlueprintCallable, Category = "BaseEnemy | Controller", meta = ( DisplayName = "Rotate towards target" ) )
	void rotatePawnTowardsTargetXY( FVector targetPosition );


	void increaseAttackCounter();

	UFUNCTION( BlueprintCallable)
	void changeCombatState( combatState newState );

	UFUNCTION()
		void whenHpGoesTo0( DamageModes type );
};
