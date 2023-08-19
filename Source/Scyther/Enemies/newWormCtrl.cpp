// Fill out your copyright notice in the Description page of Project Settings.


#include "./newWormCtrl.h"
#include "./newWorm.h"
#include <GameFramework/FloatingPawnMovement.h>
#include <Scyther/Components/DropLooteablesComponent.h>
#include <Scyther/ScytherGameModeBase.h>
#include <Kismet/GameplayStatics.h>
#include <Scyther/ScytherGameModeBase.h>
#include <Scyther/Player/ScytherPlayerPawn.h>
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include <DrawDebugHelpers.h>

AnewWormCtrl::AnewWormCtrl(){}

void AnewWormCtrl::BeginPlay()
{
	Super::BeginPlay();

	wormPawn = Cast<AnewWorm>( this->GetPawn() );
	toggleGoToPlayer( false );

	gm = Cast<AScytherGameModeBase>( UGameplayStatics::GetGameMode( GetWorld() ) );

	changeState( WormStates::underGround );

}

void AnewWormCtrl::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if( wormPawn->is_AI_Active )
	{
		switch( currentState )
		{
			case WormStates::underGround:
			{
				currentUndergroundTimer += DeltaTime;
				break;
			}
			case WormStates::verticalAttack:
			{
				currentPreparationTimer += DeltaTime;

				if( currentPreparationTimer >= verticalAttackPreparationTimer )
				{
					isPreparingAttack = false;
				}
				break; 
			}
			case WormStates::horizontalAttack: {
				//Fail safe in case worm cant get to player
				if (currentPreparationTimer >= maxHAttackPreparationTimer)
				{
					isPreparingAttack = false;
				}

				APawn* targetPawn = Cast<APawn>(UGameplayStatics::GetActorOfClass(GetWorld(), gm->aiPlayerBPClass));
				if (targetPawn == nullptr)
				{
					targetPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
				}

				FVector playerLocation = targetPawn->GetActorLocation();
				playerLocation.Z = 0;
				FVector wormLocation = wormPawn->GetActorLocation();
				wormLocation.Z = 0;

				float distanceBetweenBoth = FMath::Abs((wormLocation - playerLocation).Size());
				if (distanceBetweenBoth <= distanceToStartHorizontalAttackMontage) {
					isPreparingAttack = false;
				}

				break;
			}
			case WormStates::movingToLocation:
			{
				SavedLocation.Z = wormPawn->GetActorLocation().Z;

				if( SavedLocation.Equals( wormPawn->GetActorLocation(), 350 ))
				{
					wormPawn->SetActorLocation( SavedLocation );
					changeState( WormStates::underGround );

					if( currentCombatState == combatState::goingToIdle )
					{
						changeCombatState( combatState::idle );
					}
					else if( currentCombatState == combatState::goingToCombat )
					{
						changeCombatState( combatState::inCombat );
					}

					
				}
			}
		}
	}
}

void AnewWormCtrl::PickNewWormAttack()
{
	if( currentUndergroundTimer >= timeUnderground )
	{
		changeState( !wormPawn->isLightMode ? WormStates::horizontalAttack : WormStates::verticalAttack );

		if( currentCombatState != combatState::inCombat )
		{
			changeCombatState( combatState::inCombat );
		}
	}
}

void AnewWormCtrl::changeState( WormStates newState )
{
	if( newState == WormStates::underGround )
	{
		wormPawn->toUndergroundMode();
	}
	else if( newState  != WormStates::movingToLocation && currentCombatState == combatState::inCombat)
	{
		currentUndergroundTimer = 0;
		currentPreparationTimer = 0;
		isPreparingAttack = true;

		//Set worm location to player, so the vertical attack always starts close to player
		if (newState == WormStates::verticalAttack) {
			APawn* targetPawn = Cast<APawn>(UGameplayStatics::GetActorOfClass(GetWorld(), gm->aiPlayerBPClass));
			if (targetPawn == nullptr)
			{
				targetPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
			}
			wormPawn->SetActorLocation(FVector(targetPawn->GetActorLocation().X, targetPawn->GetActorLocation().Y, wormPawn->GetActorLocation().Z));
		}
		//Find the location close to the player that is available
		else {
			FEnvQueryRequest EQS_Request = FEnvQueryRequest(getStartHorizontalAttackLocation_Path, GetPawn());
			EQS_Request.SetFloatParam("Distance.FloatValueMin", distanceToStartHorizontalAttack);

			EQS_Request.Execute(EEnvQueryRunMode::RandomBest25Pct, this, &AnewWormCtrl::CheckHStartPositionEQSResult);

		}
		
	}

	currentState = newState;
}

void AnewWormCtrl::CheckHStartPositionEQSResult(TSharedPtr<FEnvQueryResult> result)
{
	if (result->IsSuccsessful()) {
		wormPawn->SetActorLocation(FVector(result->GetItemAsLocation(0).X, result->GetItemAsLocation(0).Y, wormPawn->GetActorLocation().Z));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("COUDNT START HORIZONTAL ATTACK"));
		changeState(WormStates::verticalAttack);
	}
}

void AnewWormCtrl::toggleGoToPlayer( bool active)
{
	if( active )
	{
		if( !wormPawn->movementComp->IsActive() )
		{
			wormPawn->movementComp->Activate();
			wormPawn->movementComp->MaxSpeed = currentState == WormStates::verticalAttack ? VAttackMovSpeed : HAttackMovSpeed;
		}

		APawn* targetPawn = Cast<APawn>( UGameplayStatics::GetActorOfClass( GetWorld(), gm->aiPlayerBPClass ) );
		if( targetPawn == nullptr )
		{
			targetPawn = UGameplayStatics::GetPlayerPawn( GetWorld(), 0 );
		}
		
		MoveToActor( targetPawn, 0, false );
	}
	else
	{
		wormPawn->movementComp->Deactivate();
		wormPawn->movementComp->MaxSpeed = 0;
	}
}

void AnewWormCtrl::toggleParticles( bool active )
{
	if( active )
	{
		if( stonesVFXcomp == nullptr || !stonesVFXcomp->IsActive() )
		{
			stonesVFXcomp = UNiagaraFunctionLibrary::SpawnSystemAttached( wormPawn->stonesVFX, wormPawn->GetRootComponent(), NAME_None, FVector( 0.f ), FRotator( 0.f ), EAttachLocation::Type::KeepRelativeOffset, true );
			stonesVFXcomp->Activate();
			stonesVFXcomp->SetVisibility( true, false );
		}

		if( currentState == WormStates::verticalAttack )
		{
			
			stonesVFXcomp->SetWorldScale3D(
				FMath::Lerp( FVector( 0.1, 0.1, 0.1 ), FVector( 1, 1, 1 ), currentPreparationTimer / verticalAttackPreparationTimer )
			);
		}
		else if( currentState == WormStates::horizontalAttack )
		{
			stonesVFXcomp->SetWorldScale3D(
				FVector( 0.6, 0.6, 0.6)
			);
		}
		else
		{
			stonesVFXcomp->SetWorldScale3D(
				FVector( 0.2, 0.2, 0.2 )
			);
		}
	}
	else
	{
		if( stonesVFXcomp != nullptr )
		{
			stonesVFXcomp->SetVisibility(false, false);
			stonesVFXcomp->Deactivate();
		}
	}
	
}

void AnewWormCtrl::DoAttack( bool aimToPlayer )
{	
	if( aimToPlayer )
	{
		APawn* targetPawn = Cast<APawn>( UGameplayStatics::GetActorOfClass( GetWorld(), gm->aiPlayerBPClass ) );
		if( targetPawn == nullptr )
		{
			targetPawn = UGameplayStatics::GetPlayerPawn( GetWorld(), 0 );
		}

		FVector playerLocation = targetPawn->GetActorLocation();
		playerLocation.Z = 0;
		FVector wormLocation = wormPawn->GetActorLocation();
		wormLocation.Z = 0;

		FVector traceVector = playerLocation - wormLocation;
		wormPawn->SetActorRotation( traceVector.Rotation() );
		wormPawn->AddActorWorldRotation( FRotator( 0, -90, 0 ) );

		if( currentState == WormStates::verticalAttack )
		{
			wormPawn->doVerticalAttack();
		}
		else
		{
			wormPawn->doHorizontalAttack();
		}

		wormPawn->enableDamageArea();
	}
	else
	{
		if( currentState == WormStates::verticalAttack )
		{
			wormPawn->doVerticalAttack();
		}
		else
		{
			wormPawn->doHorizontalAttack();
		}
	}

}


void AnewWormCtrl::FinishAttackPreparation()
{

	toggleParticles( false );
	toggleGoToPlayer( false );


	wormPawn->toOverworldMode();
}

void AnewWormCtrl::AttackFinished()
{
	changeState( WormStates::underGround );

	if( currentCombatState == combatState::inCombat )
	{
		increaseAttackCounter();
	}
	else if( currentCombatState == combatState::idle )
	{
		changeCombatState( combatState::goingToIdle );
	}
}

void AnewWormCtrl::findIdleLocation()
{
	FEnvQueryRequest EQS_Request = FEnvQueryRequest( getStartHorizontalAttackLocation_Path, GetPawn() );
	EQS_Request.SetFloatParam( "Distance.FloatValueMin", distanceToStartHorizontalAttack*1.5 );

	EQS_Request.Execute( EEnvQueryRunMode::RandomBest25Pct, this, &AnewWormCtrl::CheckIdleLocation );
}

void AnewWormCtrl::CheckIdleLocation( TSharedPtr<FEnvQueryResult> result )
{
	if( result->IsSuccsessful() )
	{
		SavedLocation = result->GetItemAsLocation( 0 );
		changeState( WormStates::movingToLocation );
	}
}

void AnewWormCtrl::moveToSavedLocation()
{
	if( !wormPawn->movementComp->IsActive() )
	{
		wormPawn->movementComp->Activate();
		wormPawn->movementComp->MaxSpeed = VAttackMovSpeed * 2;
	}
	MoveToLocation( SavedLocation, 1, false );
}
