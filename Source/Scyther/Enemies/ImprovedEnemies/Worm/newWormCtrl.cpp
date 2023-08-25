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

AnewWormCtrl::AnewWormCtrl()
{}

void AnewWormCtrl::BeginPlay()
{
	Super::BeginPlay();

	wormPawn = Cast<AnewWorm>(this->GetPawn());
	gm = Cast<AScytherGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	stonesVFXcomp = UNiagaraFunctionLibrary::SpawnSystemAttached(wormPawn->stonesVFX, wormPawn->GetRootComponent(), NAME_None, FVector(0.f), FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, true);
	ShowParticles(false);

	changeWormState(WormStates::underGround);
}

void AnewWormCtrl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (wormPawn->is_AI_Active)
	{
		currentTimer += DeltaTime;

		switch (currentState)
		{
		case WormStates::underGround:
		{

			break;
		}
		case WormStates::verticalAttack:
		{
			if (currentCombatState == combatState::inCombat && !wormPawn->isInAttackAnimation)
			{
				copyPlayerRotation();
			}
			else if (currentCombatState != combatState::goingToCombat && !wormPawn->isInAttackAnimation)
			{
				rotatePawnTowardsTargetXY(EQS_resultLocation);
			}

			if (currentTimer > vAttackPrepTimer && isPreparingAttack)
			{
				AttackPreparationFinished();
			}
			break;
		}
		case WormStates::horizontalAttack:
		{
			if( !wormPawn->isInAttackAnimation )
			{
				rotatePawnTowardsTargetXY( EQS_locationContext );
				wormPawn->AddActorWorldRotation( FRotator( 0, -90, 0 ) );
			}

			if( isPreparingAttack && 
				(FVector::Distance( wormPawn->GetActorLocation(), EQS_locationContext ) <= hAttackPlayMontageDistance ||
				currentTimer > hAttackMaxPrepTimer ) )
			{
				AttackPreparationFinished();
			}

			break;
		}
		case WormStates::movingToLocation:
		{
			if (wormPawn->GetActorLocation().Equals(EQS_resultLocation, 100))
			{
				wormPawn->SetActorLocation(EQS_resultLocation);
				changeWormState(WormStates::underGround);
				changeCombatState(combatState::idle);
			}
			break;
		}
		}
	}
}

void AnewWormCtrl::changeWormState(WormStates newState)
{
	WormStates previousState = currentState;
	currentState = newState;

	switch (newState)
	{
	case WormStates::underGround:
	{
		wormPawn->toUndergroundMode();
		stopWormMovement();
		currentTimer = 0;
		break;
	}
	case WormStates::verticalAttack:
	{
		isPreparingAttack = true;
		currentTimer = 0;

		if (currentCombatState == combatState::inCombat || currentCombatState == combatState::goingToCombat)
		{
			AScytherPlayerPawn* playerPawn = gm->getCorrectPlayerPawn();

			FVector locationToTp = playerPawn->GetActorLocation();
			locationToTp.Z = wormPawn->GetActorLocation().Z;

			wormPawn->SetActorLocation(locationToTp);
		}
		else if (currentCombatState != combatState::goingToCombat)
		{
			EQS_resultLocation.Z = wormPawn->GetActorLocation().Z;
			wormPawn->SetActorLocation(EQS_resultLocation);
		}
		break;
	}
	case WormStates::horizontalAttack:
	{
		isPreparingAttack = true;
		currentTimer = 0;

		if( currentCombatState == combatState::inCombat || currentCombatState == combatState::goingToCombat )
		{
			getHAttackStartLocation( gm->getCorrectPlayerPawn()->GetActorLocation() );
		}
		else
		{
			getHAttackStartLocation( wormPawn->GetActorLocation() );
		}
		

		break;
	}
	case WormStates::movingToLocation:
	{
		wormPawn->toUndergroundMode();
		break;
	}
	}
}

void AnewWormCtrl::stopWormMovement()
{
	wormPawn->movementComp->MaxSpeed = 0;
	wormPawn->movementComp->StopMovementImmediately();
	wormPawn->movementComp->Deactivate();
}

void AnewWormCtrl::ShowParticles(bool activate)
{
	if (!isVFXactive && !activate)
	{
		return;
	}
	else
	{
		isVFXactive = activate;
		stonesVFXcomp->SetVisibility(activate, false);

		if (activate)
		{
			switch (currentState)
			{
			case WormStates::verticalAttack:
			{
				stonesVFXcomp->SetWorldScale3D(
					FMath::Lerp(vAttackMinVFXsize, vAttackMaxVFXsize, currentTimer / vAttackPrepTimer)
				);
				break;
			}
			case WormStates::horizontalAttack:
			{
				stonesVFXcomp->SetWorldScale3D(
					hAttackVFXsize
				);
				break;
			}
			default:
			{
				stonesVFXcomp->SetWorldScale3D(
					toIdleVFXsize
				);

				break;
			}
			}
		}
	}
}

void AnewWormCtrl::combatStateChanged()
{
	switch (currentCombatState) {
	case combatState::inCombat: {
		currentAttacksLeftToidle = 0;
		
		if (currentState == WormStates::verticalAttack ||
			currentState == WormStates::horizontalAttack) {
			//If its currently doing an attack, it must be in an idle position doing an idle attack. We are going to wait for this attack to finish.
			//When this attack animations ends, it will count as one attack under inCombat state, so we reduce the current count to not reduce the 
			//number of real attacks during in Combat
			if (wormPawn->isInAttackAnimation) {
				isDoingDummyAttack = true;
			}
			//If its already preparing for an attack, it must stop the preparation and start the attack from 0 from player location
			else if (isPreparingAttack) {
				changeWormState(WormStates::underGround);
			}
		}
		break;
	}
	}
}

void AnewWormCtrl::AttackPreparationFinished()
{
	wormPawn->toOverworldMode();
	isPreparingAttack = false;
	ShowParticles(false);
	stopWormMovement();
}

void AnewWormCtrl::AttackFinished()
{
	changeWormState(WormStates::underGround);

	if (isDoingDummyAttack) {
		isDoingDummyAttack = false;
		return;
	}
	if (currentCombatState == combatState::idle)
	{
		changeCombatState(combatState::goingToIdle);
	}
	else
	{
		increaseAttackCounter();
	}
}

void AnewWormCtrl::TryChangeToAttackState(bool ignoreTimer)
{
	//Ignore timer is true when the worm is trying to attack "outside of the arena"
	if (ignoreTimer)
	{
		changeWormState(!wormPawn->isLightMode ? WormStates::horizontalAttack : WormStates::verticalAttack);
	}
	else {
		if (currentTimer >= undergroundTimer)
		{
			//The attack only starts when the combatManager updates canStartAttack bool
			if (canStartAttack) {
				canStartAttack = false;
				isExecutingAttack = true;
				changeWormState(!wormPawn->isLightMode ? WormStates::horizontalAttack : WormStates::verticalAttack);
			}
		}
	}
		
}

void AnewWormCtrl::DoAttack()
{
	ShowParticles( false );

	if (currentState == WormStates::verticalAttack) {
		wormPawn->doVerticalAttack();
	}
	else
	{
		wormPawn->doHorizontalAttack();
	}
	wormPawn->enableDamageArea();

	removeCtrlFromAttackPool();
}

void AnewWormCtrl::moveToPlayer()
{
	if (currentCombatState == combatState::inCombat)
	{
		if (!wormPawn->movementComp->IsActive())
		{
			wormPawn->movementComp->Activate();

			wormPawn->movementComp->MaxSpeed = currentState == WormStates::verticalAttack ? vAttackSpeed : hAttackSpeed;
		}

		APawn* targetPawn = Cast<APawn>(UGameplayStatics::GetActorOfClass(GetWorld(), gm->aiPlayerBPClass));
		if (targetPawn == nullptr)
		{
			targetPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		}
		FVector targetLocation = targetPawn->GetActorLocation();
		targetLocation.Z = wormPawn->GetActorLocation().Z;

		MoveToLocation(targetLocation, 1, false);
	}
	else if( currentState == WormStates::horizontalAttack )
	{
		MoveToLocation( EQS_locationContext, 1, false );
	}
}

void AnewWormCtrl::getHAttackStartLocation(FVector locationContext)
{
	FVector loc = locationContext;
	loc.Z = wormPawn->GetActorLocation().Z;

	EQS_locationContext = loc;

	if( currentCombatState == combatState::inCombat && currentCombatState == combatState::goingToCombat )
	{
		FEnvQueryRequest EQS_Request = FEnvQueryRequest( EQS_ObtainNavigableLocationAtDistance, GetPawn() );
		EQS_Request.SetFloatParam( "Distance.FloatValueMin", hAttackStartDistance );

		EQS_Request.Execute( EEnvQueryRunMode::RandomBest25Pct, this, &AnewWormCtrl::parseHAttackStartLocation );
	}
	else
	{
		FEnvQueryRequest EQS_Request = FEnvQueryRequest( EQS_ObtainIdleHAttackLocation, GetPawn() );
		EQS_Request.SetFloatParam( "Distance.FloatValueMin", hAttackStartDistance );

		EQS_Request.Execute( EEnvQueryRunMode::RandomBest25Pct, this, &AnewWormCtrl::parseHAttackStartLocation );
	}
	
}

void AnewWormCtrl::parseHAttackStartLocation( TSharedPtr<FEnvQueryResult> result )
{
	if( result->IsSuccsessful() )
	{
		EQS_resultLocation = result->GetItemAsLocation( 0 );
		EQS_resultLocation.Z = wormPawn->GetActorLocation().Z;

		wormPawn->SetActorLocation( EQS_resultLocation );

	}
	else
	{
		EQS_resultLocation = wormPawn->GetActorLocation();
		GEngine->AddOnScreenDebugMessage( -1, 20, FColor::Red, FString( "parseHAttackStartLocation unsuccsessful result" ) );
	}
}

void AnewWormCtrl::obtainIdleLocation()
{
	FVector loc = gm->getCorrectPlayerPawn()->GetActorLocation();
	loc.Z = wormPawn->GetActorLocation().Z;

	EQS_locationContext = loc;
	FEnvQueryRequest EQS_Request = FEnvQueryRequest(EQS_ObtainNavigableLocationAtDistance, GetPawn());
	EQS_Request.SetFloatParam("Distance.FloatValueMin", hAttackStartDistance * 1.5);

	EQS_Request.Execute(EEnvQueryRunMode::RandomBest25Pct, this, &AnewWormCtrl::moveToIdleLocation);
}

void AnewWormCtrl::moveToIdleLocation(TSharedPtr<FEnvQueryResult> result)
{
	if (result->IsSuccsessful())
	{
		EQS_resultLocation = result->GetItemAsLocation(0);
		EQS_resultLocation.Z = wormPawn->GetActorLocation().Z;
	}
	else
	{
		EQS_resultLocation = wormPawn->GetActorLocation();
		GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Red, FString("MoveToIdleLocation unsuccsessful result"));
	}

	if (!wormPawn->movementComp->IsActive())
	{
		wormPawn->movementComp->Activate();
		wormPawn->movementComp->MaxSpeed = goingToIdleSpeed;
	}

	MoveToLocation(EQS_resultLocation, 1, false);
	changeWormState(WormStates::movingToLocation);
}

void AnewWormCtrl::copyPlayerRotation()
{
	AScytherPlayerPawn* playerPawn = gm->getCorrectPlayerPawn();
	wormPawn->SetActorRotation(playerPawn->GetActorRotation());
	wormPawn->AddActorWorldRotation(FRotator(0, -90, 0));
}
