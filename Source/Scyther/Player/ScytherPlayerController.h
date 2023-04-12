// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScytherPlayerPawn.h"
#include "GameFramework/PlayerController.h"
#include "RespawnManager.h"
#include "ScytherPlayerController.generated.h"

/**
 *
 */
UCLASS()
class SCYTHER_API AScytherPlayerController: public APlayerController
{
	GENERATED_BODY()

public:
	//Contructor
	AScytherPlayerController();

protected:
	virtual void BeginPlay() override;


public:
	//ATRIBUTTES

	UPROPERTY( Transient, EditAnywhere, BlueprintReadWrite );
	AScytherPlayerPawn* scytherPlayer;

	//METHODS
	virtual void Tick( float DeltaTime ) override;

	virtual void SetupInputComponent();


	//Input methods

	void CallMoveRight( float value );

	void CallMoveUp( float value );

	void CallMoveCameraUpDown( float value );

	void CallTargetSwitchRight( float value );
	void CallTargetSwitchUp( float value );

	void CallJumpPressed();
	void CallJumpReleased();

	void CallAttackPressed();

	void CallDashPressed();

	void CallPausePressed();

	void CallTargetPressed();

	void CallSkillAPressed();
	void CallSkillBPressed();

	void CallGrappleHookPressed();

	void CallGODModePressed();

	//void MoveToTheNextPos();
	//void MoveToThePreviousPos();

	UPROPERTY( BlueprintReadOnly, Category = "HUD" )
		//TSubclassOf<class AHUD> hudClass;
		class AScytherHUD* hud;
};
