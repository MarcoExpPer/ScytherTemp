// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScytherPlayerPawn.h"
#include "GameFramework/PlayerController.h"
#include "RespawnManager.h"
#include "ScytherPlayerController.generated.h"

class ARespawnManager;
/**
 *
 */
UCLASS()
class SCYTHER_API AScytherPlayerController: public APlayerController
{
	GENERATED_BODY()
private:
	ARespawnManager *respawnMan;

public:
	float inputRight;
	float inputDown;

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
	UFUNCTION( BlueprintCallable )
	void CallMoveRight( float value );

	UFUNCTION( BlueprintCallable )
	void CallMoveUp( float value );

	UFUNCTION( BlueprintCallable )
	void CallMoveCameraUpDown( float value );

	UFUNCTION( BlueprintCallable )
	void CallTargetSwitchRight( float value );
	UFUNCTION( BlueprintCallable )
	void CallTargetSwitchUp( float value );

	UFUNCTION( BlueprintCallable )
	void CallJumpPressed();
	UFUNCTION( BlueprintCallable )
	void CallJumpReleased();

	UFUNCTION( BlueprintCallable )
	void CallAttackPressed();

	UFUNCTION( BlueprintCallable )
	void CallDashPressed(); 

	UFUNCTION( BlueprintCallable )
	void CallBloodRecoverPressed();

	UFUNCTION( BlueprintCallable )
	void CallPausePressed();

	UFUNCTION( BlueprintCallable )
	void CallTargetPressed();

	UFUNCTION( BlueprintCallable )
	void CallSkillAPressed();
	UFUNCTION( BlueprintCallable )
	void CallSkillBPressed();

	UFUNCTION( BlueprintCallable )
	void CallGrappleHookPressed();

	UFUNCTION( BlueprintCallable )
	void CallGODModePressed();

	UFUNCTION( BlueprintCallable )
	void MoveToTheNextPos();
	UFUNCTION( BlueprintCallable )
	void MoveToThePreviousPos();

	UFUNCTION( BlueprintCallable )
	void RespawnPlayer();

	UPROPERTY( BlueprintReadOnly, Category = "HUD" )
		//TSubclassOf<class AHUD> hudClass;
		class AScytherHUD* hud;
};
