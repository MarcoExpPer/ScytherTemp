// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Scyther/Enemies/BaseEnemyCtrl.h>
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "newWormCtrl.generated.h"

UENUM( BlueprintType )
enum class WormStates: uint8
{
	underGround,
	verticalAttack,
	horizontalAttack,
	movingToLocation
};

UCLASS()
class SCYTHER_API AnewWormCtrl: public ABaseEnemyCtrl
{
	GENERATED_BODY()

public:
	//Pawn
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
		class AnewWorm* wormPawn;

	//State
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
		WormStates currentState = WormStates::underGround;

	//Timers
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float undergroundTimer = 2.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float vAttackPrepTimer = 3.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float hAttackMaxPrepTimer = 5.f;

	//Speed
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float vAttackSpeed = 400;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float hAttackSpeed = 450;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float goingToIdleSpeed = 600;

	//Vertical attack
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FVector vAttackMinVFXsize = FVector( 0.1, 0.1, 0.1 );
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FVector vAttackMaxVFXsize = FVector( 0.9, 0.9, 0.9 );

	//Horizontal Attack
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float hAttackStartDistance = 850;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float hAttackPlayMontageDistance = 450;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FVector hAttackVFXsize = FVector( 0.5, 0.5, 0.5 );

	//MoveToIdle
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FVector toIdleVFXsize = FVector( 0.25, 0.25, 0.25 );

	//EQS
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		UEnvQuery* EQS_ObtainNavigableLocationAtDistance;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		UEnvQuery* EQS_ObtainIdleHAttackLocation;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FVector EQS_locationContext;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FVector EQS_resultLocation;



private:
	class AScytherGameModeBase* gm;
	UNiagaraComponent* stonesVFXcomp;

	bool isVFXactive = true;

	float currentTimer = 0.f;
	//To know if the attack the worm is finishing is a dummy attack that started before being in combate
	bool isDoingDummyAttack = false;
	bool isPreparingAttack = false;

	AnewWormCtrl();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick( float DeltaTime ) override;

	//General functions

	UFUNCTION( Blueprintcallable )
		void changeWormState( WormStates newState );


	UFUNCTION( Blueprintcallable )
		bool getIsPreparingAttack() { return isPreparingAttack; }


	UFUNCTION( Blueprintcallable )
		void stopWormMovement();

	UFUNCTION( Blueprintcallable )
		void ShowParticles( bool activate );

	//AttackGeneral
	virtual void combatStateChanged() override;

	UFUNCTION( Blueprintcallable )
		void AttackPreparationFinished();

	UFUNCTION( Blueprintcallable )
		void AttackFinished();

	UFUNCTION( Blueprintcallable )
		void TryChangeToAttackState(bool ignoreTimer = false);

	UFUNCTION( Blueprintcallable )
		void DoAttack();

	//Vertical Attack
	UFUNCTION( Blueprintcallable )
		void moveToPlayer();

	//Horizontal attack
	UFUNCTION( Blueprintcallable )
		void getHAttackStartLocation( FVector locationContext );

	void parseHAttackStartLocation( TSharedPtr<FEnvQueryResult> result );
	//GoingToCombate
		//Nothing, just wait a second and change to combat


	//Going to Idle
	UFUNCTION( Blueprintcallable )
		void obtainIdleLocation();

		void moveToIdleLocation( TSharedPtr<FEnvQueryResult> result );

	//Idle do an attack on the air

private:
	void copyPlayerRotation();
	//void CheckHStartPositionEQSResult( TSharedPtr<FEnvQueryResult> result );
};
