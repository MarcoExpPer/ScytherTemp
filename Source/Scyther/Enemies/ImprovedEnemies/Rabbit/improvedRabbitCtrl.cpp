// Fill out your copyright notice in the Description page of Project Settings.


#include "./improvedRabbitCtrl.h"
#include "./improvedRabbit.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include <Scyther/ScytherGameModeBase.h>
#include <Scyther/Player/ScytherPlayerPawn.h>
#include <Scyther/Components/HealthComponent.h>
#include <DrawDebugHelpers.h>

AimprovedRabbitCtrl::AimprovedRabbitCtrl(){}

void AimprovedRabbitCtrl::BeginPlay()
{
	Super::BeginPlay();
	rabbitPawn = Cast<AimprovedRabbit>( this->GetPawn() );
	updateIsPermanentIdle( rabbitPawn->isLightMode );
	rabbitPawn->turnDarkEvent.AddDynamic( this, &AimprovedRabbitCtrl::rabbitTurnedDark );
}

void AimprovedRabbitCtrl::Tick( float DeltaTime )
{
	if( rabbitPawn->is_AI_Active )
	{
		//GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Green, GetStateAsName( currentRabbitState ) );
	}
}

void AimprovedRabbitCtrl::changeRabbitState( improvedRabbitState newState )
{
	if( newState == currentRabbitState )
	{
		return;
	}
	currentRabbitState = newState;

	switch( newState )
	{
		case improvedRabbitState::runningAway:
		{
			updateIsPermanentIdle( true );
			break;
		}
		case improvedRabbitState::goingToIdle:
		{
			obtainIdleLocation();
			updateIsPermanentIdle( false );
			break;
		}
		case improvedRabbitState::waitToAttack : {
			rabbitPawn->changeAnimation( rabbitAnimations::NONE );
			rabbitPawn->disableMovement( false );

			updateIsPermanentIdle( false );
			break;
		}
		case improvedRabbitState::attacking:
		{
			isExecutingAttack = true;
			rabbitPawn->isPreparingAttack = true;
			attackTarget.Z = rabbitPawn->GetActorLocation().Z;

			MoveToLocation( attackTarget, 10, true, true );

			updateIsPermanentIdle( false );
			break;
		}
		default:
		{
			updateIsPermanentIdle( false );
			break;
		}
	}
	
	
}

void AimprovedRabbitCtrl::combatStateChanged()
{
	switch( currentCombatState )
	{
	case combatState::goingToCombat:
	{
		changeRabbitState( improvedRabbitState::goingToPlayer );
		break;
	}
	case combatState::goingToIdle:
	{
		if(!rabbitPawn ->isLightMode)
			changeRabbitState( improvedRabbitState::goingToIdle );
		break;
	}
	case combatState::idle:
	{
		if( !rabbitPawn->isLightMode 
			&& EQS_locationContext.Equals( FVector(0, 0, 0)) )
		{
			changeCombatState( combatState::goingToIdle );
		}
	}
	}
}

void AimprovedRabbitCtrl::updateRunAwayLocation()
{
	FEnvQueryRequest EQS_Request = FEnvQueryRequest( EQS_GetRunAwayLocation, GetPawn() );
	EQS_Request.Execute( EEnvQueryRunMode::RandomBest5Pct, this, &AimprovedRabbitCtrl::ranAwayEQSresult );
}


void AimprovedRabbitCtrl::ranAwayEQSresult( TSharedPtr<FEnvQueryResult> result )
{
	if( result->IsSuccsessful() )
	{
		FVector resV = result->GetItemAsLocation( 0 );
		rabbitPawn->flockToThisLocation( resV );
	}
	else
	{
		GEngine->AddOnScreenDebugMessage( -1, 10, FColor::Red, FString( TEXT( "ERROR WHILE DOING RUN AWAY EQS" ) ) );
	}
}

void AimprovedRabbitCtrl::rabbitTurnedDark()
{
	changeRabbitState( improvedRabbitState::goingToIdle );
}

void AimprovedRabbitCtrl::moveToPlayer()
{
	rabbitPawn->flockToThisLocation( gm->getCorrectPlayerPawn()->GetActorLocation());
}

bool AimprovedRabbitCtrl::isCloseToPlayer()
{
	return FVector::Dist2D( gm->getCorrectPlayerPawn()->GetActorLocation(), rabbitPawn->GetActorLocation()) <= rabbitPawn->AT_range * 0.8;
}

void AimprovedRabbitCtrl::attackInterrupted()
{
	changeCombatState( combatState::goingToCombat );
	removeCtrlFromAttackPool();

	isExecutingAttack = false;
}

void AimprovedRabbitCtrl::savePlayerLocation()
{
	FVector traceToPlayer = gm->getCorrectPlayerPawn()->GetActorLocation() - rabbitPawn->GetActorLocation();
	traceToPlayer.Normalize();
	
	traceToPlayer *= rabbitPawn->AT_range * 1.1f;
	attackTarget = rabbitPawn->GetActorLocation() + traceToPlayer;

	rotatePawnTowardsTargetXY( attackTarget );
}

void AimprovedRabbitCtrl::attackFinished()
{
	changeCombatState( combatState::goingToCombat );

	removeCtrlFromAttackPool();
	isExecutingAttack = false;

	increaseAttackCounter();
}

void AimprovedRabbitCtrl::obtainIdleLocation()
{
	FVector loc = gm->getCorrectPlayerPawn()->GetActorLocation();
	loc.Z = rabbitPawn->GetActorLocation().Z;

	EQS_locationContext = loc;
	FEnvQueryRequest EQS_Request = FEnvQueryRequest( EQS_ObtainNavigableLocationAtDistance, GetPawn() );
	EQS_Request.SetFloatParam( "Distance.FloatValueMin", 700 );

	EQS_Request.Execute( EEnvQueryRunMode::RandomBest25Pct, this, &AimprovedRabbitCtrl::moveToIdleLocation );
}

void AimprovedRabbitCtrl::moveToIdleLocation( TSharedPtr<FEnvQueryResult> result )
{
	if( result->IsSuccsessful() )
	{
		idleLocation = result->GetItemAsLocation( 0 );
		rotatePawnTowardsTargetXY( idleLocation );
		rabbitPawn->flockToThisLocation( idleLocation );
		
	}
	else{
	
		GEngine->AddOnScreenDebugMessage( -1, 20, FColor::Red, FString( "Rabbit MoveToIdleLocation unsuccsessful result" ) );
	}
}

bool AimprovedRabbitCtrl::checkDistanceToIdleLocation()
{
	float distance = FVector::Dist2D( idleLocation, rabbitPawn->GetActorLocation() );
	return distance <= 150;
}

void AimprovedRabbitCtrl::MoveToIdleAgain()
{
	if( !idleLocation.Equals( FVector( 0, 0, 0 )) )
	{
		rotatePawnTowardsTargetXY( idleLocation );
		rabbitPawn->flockToThisLocation( idleLocation );
	}
}

FString AimprovedRabbitCtrl::GetStateAsName( improvedRabbitState EnumValue )
{
	const UEnum* EnumPtr = FindObject<UEnum>( ANY_PACKAGE, TEXT( "improvedRabbitState" ), true );
	if( !EnumPtr ) return FString( "Invalid" );
	return EnumPtr->GetNameByValue( (int64) EnumValue ).ToString();
}