// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Scyther/Enemies/BaseEnemyCtrl.h>
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "improvedRabbitCtrl.generated.h"

UENUM( BlueprintType )
enum class improvedRabbitState: uint8
{
	runningAway,
	goingToPlayer,
	goingToIdle,
	waitToAttack,
	attacking,
};



UCLASS()
class SCYTHER_API AimprovedRabbitCtrl: public ABaseEnemyCtrl
{
	GENERATED_BODY()
public:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
		class AimprovedRabbit* rabbitPawn;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		improvedRabbitState currentRabbitState = improvedRabbitState::attacking;

	FVector attackTarget;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		UEnvQuery* EQS_GetRunAwayLocation;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		UEnvQuery* EQS_ObtainNavigableLocationAtDistance;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FVector EQS_locationContext;

	FVector idleLocation;

	AimprovedRabbitCtrl();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick( float DeltaTime ) override;

	//General functions
	UFUNCTION( Blueprintcallable )
		void changeRabbitState( improvedRabbitState  newState );

	virtual void combatStateChanged() override;

	//Run away functions
	UFUNCTION( Blueprintcallable )
		void updateRunAwayLocation();

	void ranAwayEQSresult( TSharedPtr<FEnvQueryResult> result );

	UFUNCTION()
		void rabbitTurnedDark();
	//Going to player
	UFUNCTION( Blueprintcallable )
		void moveToPlayer();
	UFUNCTION( Blueprintcallable )
		bool isCloseToPlayer();

	//Attack
		void attackInterrupted();
	UFUNCTION( Blueprintcallable )
		void savePlayerLocation();
	UFUNCTION( Blueprintcallable )
		void attackFinished();

	//Going to idle
	void obtainIdleLocation();

	void moveToIdleLocation( TSharedPtr<FEnvQueryResult> result );

	UFUNCTION( Blueprintcallable )
		bool checkDistanceToIdleLocation();

	UFUNCTION( Blueprintcallable )
		void MoveToIdleAgain();
private:
	FString GetStateAsName( improvedRabbitState EnumValue );
};

