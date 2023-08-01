// Fill out your copyright notice in the Description page of Project Settings.


#include "ScytherPlayerController.h"
#include "Components/InputComponent.h"
#include "../ScytherHUD.h"
#include "Kismet/GameplayStatics.h"
#include "./RespawnManager.h"
#include "../ScytherGameModeBase.h"

AScytherPlayerController::AScytherPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

}

//BeginPlay
void AScytherPlayerController::BeginPlay()
{
	Super::BeginPlay();

	scytherPlayer = Cast<AScytherPlayerPawn>( UGameplayStatics::GetPlayerPawn( GetWorld(), 0) ); //Get Player Character
	respawnMan = Cast<AScytherGameModeBase>( UGameplayStatics::GetGameMode( GetWorld() ))->respawnMan;
}

//Tick
void AScytherPlayerController::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}



/*
	Bind all axis and actions to input
*/
void AScytherPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check( InputComponent );

	/** Axis Mappings */

	//MoveRight
	InputComponent->BindAxis( "MoveRight", this, &AScytherPlayerController::CallMoveRight );

	//MoveUp
	InputComponent->BindAxis( "MoveUp", this, &AScytherPlayerController::CallMoveUp );

	//MoveCameraUpDown
	InputComponent->BindAxis( "MoveCameraUpDown", this, &AScytherPlayerController::CallMoveCameraUpDown );

	//SwitchTarget
	InputComponent->BindAxis( "SwitchTargetRight", this, &AScytherPlayerController::CallTargetSwitchRight );
	InputComponent->BindAxis( "SwitchTargetUp", this, &AScytherPlayerController::CallTargetSwitchUp );


	/** Actions Mappings */

	//Jump
	InputComponent->BindAction( "Jump", IE_Pressed, this, &AScytherPlayerController::CallJumpPressed );
	InputComponent->BindAction( "Jump", IE_Released, this, &AScytherPlayerController::CallJumpReleased );

	//Attack
	InputComponent->BindAction( "Attack", IE_Pressed, this, &AScytherPlayerController::CallAttackPressed );

	//Dash
	InputComponent->BindAction( "Dash", IE_Pressed, this, &AScytherPlayerController::CallDashPressed );

	//Pause & RespawnPlayer
	FInputActionBinding& pause = InputComponent->BindAction( "Pause", IE_Pressed, this, &AScytherPlayerController::CallPausePressed );
	pause.bExecuteWhenPaused = true; //EVEN THOUGH THE GAME IS PAUSED, CATCH THIS EVENT

	//Target
	InputComponent->BindAction( "Target", IE_Pressed, this, &AScytherPlayerController::CallTargetPressed );

	//Skills
	InputComponent->BindAction( "SkillA", IE_Pressed, this, &AScytherPlayerController::CallSkillAPressed );
	InputComponent->BindAction( "SkillB", IE_Pressed, this, &AScytherPlayerController::CallSkillBPressed );

	//Grapple Hook
	InputComponent->BindAction("GrappleHook", IE_Pressed, this, &AScytherPlayerController::CallGrappleHookPressed);

	//GOD Mode
	InputComponent->BindAction( "GODMode", IE_Pressed, this, &AScytherPlayerController::CallGODModePressed );

	//Next Spawn
	InputComponent->BindAction( "NextCheckPoint", IE_Pressed, this, &AScytherPlayerController::MoveToTheNextPos );

	//Previous Spawn
	InputComponent->BindAction( "LastCheckPoint", IE_Pressed, this, &AScytherPlayerController::MoveToThePreviousPos );



}


void AScytherPlayerController::MoveToTheNextPos()
{
	UE_LOG( LogTemp, Display, TEXT( "MoveToTheNextPos" ) );

	scytherPlayer->MoveToTheNextPos( scytherPlayer->GetActorTransform() );
}

void AScytherPlayerController::MoveToThePreviousPos()
{
	UE_LOG( LogTemp, Display, TEXT( "MoveToThePreviousPos" ) );
	scytherPlayer->MoveToThePreviousPos( scytherPlayer->GetActorTransform() );
}


void AScytherPlayerController::CallMoveRight( float value )
{
	//Call the Character method that does the movement
	scytherPlayer->MoveRight( value );
	inputRight = value;
}

void AScytherPlayerController::CallMoveUp( float value )
{
	//Call the Character method that does the movement
	scytherPlayer->MoveForward( value );
	inputDown = value;
}

void AScytherPlayerController::CallMoveCameraUpDown( float value )
{
	//Call the Character method that does the movement of the camera
	//playerCharacter->MoveCameraUpDown();
}

void AScytherPlayerController::CallTargetSwitchRight( float value )
{
	//Call the Character method that switch the target
	scytherPlayer->TargetSwitchRight( value );
}

void AScytherPlayerController::CallTargetSwitchUp( float value )
{
	//Call the Character method that switch the target
	scytherPlayer->TargetSwitchUp( value );
}

void AScytherPlayerController::CallJumpPressed()
{
	//Call the Character method that starts the jump
	scytherPlayer->OnJumpPressed();
}

void AScytherPlayerController::CallJumpReleased()
{
	//Call the Character method that stops the jump
	scytherPlayer->OnJumpReleased();

}

void AScytherPlayerController::CallAttackPressed()
{
	//Call the Character method that does the attack
	scytherPlayer->OnAttack();
}

void AScytherPlayerController::CallDashPressed()
{
	//Call the Character method that does the dash
	scytherPlayer->OnDash();
}

void AScytherPlayerController::CallBloodRecoverPressed()
{
	//Call the Character method that does the dash
	scytherPlayer->OnBloodRecover();
}

void AScytherPlayerController::CallPausePressed()
{
	//Call the BP_InGameHud method that enables the Pause Menu
	if( respawnMan && respawnMan->waitingForRespawn )
	{
		RespawnPlayer();
	}else{ 

		hud = Cast<AScytherHUD>( GetWorld()->GetFirstPlayerController()->GetHUD() );
		if( hud != nullptr )
		{
			hud->TogglePauseHUD();
		}
	}
}

void AScytherPlayerController::CallTargetPressed()
{
	//Call the Character method that does the targeting
	scytherPlayer->TargetPressed();
}

void AScytherPlayerController::CallSkillAPressed()
{
	//Call the Character method that activate the first skill
	scytherPlayer->EnabledSkillA();
}

void AScytherPlayerController::CallSkillBPressed()
{
	//Call the Character method that activate the second skill
	scytherPlayer->EnabledSkillB();
}

void AScytherPlayerController::CallGrappleHookPressed()
{
	scytherPlayer->ActivateGrappleHook();
}

void AScytherPlayerController::CallGODModePressed()
{
	scytherPlayer->ActivateGODMode();
}

void AScytherPlayerController::RespawnPlayer()
{
	respawnMan->RestartController();
}
