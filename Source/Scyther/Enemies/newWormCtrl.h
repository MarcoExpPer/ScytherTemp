// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "./BaseEnemyCtrl.h"
#include "newWormCtrl.generated.h"

UENUM( BlueprintType )
enum class WormStates: uint8
{
	underGround,
	verticalAttack,
	horizontalAttack
};

UCLASS()
class SCYTHER_API AnewWormCtrl : public ABaseEnemyCtrl
{
	GENERATED_BODY()
	
public:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
		class AnewWorm* wormPawn;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
		WormStates currentState = WormStates::underGround;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float timeUnderground = 1.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float verticalAttackPreparationTimer = 3.f;


private:
	float currentUndergroundTimer = 0.f;
	float currentPreparationTimer = 0;
	bool isPreparingAttack = false;

	class AScytherGameModeBase* gm;

	UNiagaraComponent* stonesVFXcomp;

	AnewWormCtrl();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick( float DeltaTime ) override;

	UFUNCTION(Blueprintcallable)
	void PickNewWormAttack();

	UFUNCTION( Blueprintcallable )
	void changeState( WormStates newState);

	UFUNCTION( Blueprintcallable )
	bool getIsPreparingAttack(){ return isPreparingAttack; }

	UFUNCTION( Blueprintcallable )
	void toggleGoToPlayer( bool active, float speed = 0);

	UFUNCTION( Blueprintcallable )
	void toggleParticles( bool active);

	UFUNCTION( Blueprintcallable )
	float DoAttack();
	UFUNCTION( Blueprintcallable )
	void finishVerticalPreparation();

	

	
};
