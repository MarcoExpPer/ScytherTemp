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
		}
	}
}

void AnewWormCtrl::PickNewWormAttack()
{
	if( currentUndergroundTimer >= timeUnderground )
	{
		//changeState( wormPawn->isLightMode ? WormStates::horizontalAttack : WormStates::verticalAttack );
		changeState( WormStates::verticalAttack );
	}
}

void AnewWormCtrl::changeState( WormStates newState )
{
	if( newState == WormStates::underGround )
	{
		wormPawn->toUndergroundMode();
	}
	else
	{
		currentUndergroundTimer = 0;
		currentPreparationTimer = 0;
		isPreparingAttack = true;
	}

	currentState = newState;
}

void AnewWormCtrl::toggleGoToPlayer( bool active, float speed)
{
	if( active )
	{
		if( !wormPawn->movementComp->IsActive() )
		{
			wormPawn->movementComp->Activate();
			wormPawn->movementComp->MaxSpeed = speed;
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
		else
		{
			stonesVFXcomp->SetWorldScale3D(
				FVector( 1, 1, 1 )
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

float AnewWormCtrl::DoAttack()
{
	APawn* targetPawn = Cast<APawn>(UGameplayStatics::GetActorOfClass(GetWorld(), gm->aiPlayerBPClass));
	if (targetPawn == nullptr)
	{
		targetPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}

	wormPawn->SetActorRotation(targetPawn->GetActorRotation());
	wormPawn->AddActorWorldRotation(FRotator(0, -90, 0));

	if (currentState == WormStates::verticalAttack) {
		return wormPawn->doVerticalAttack();
	}
	else {
		return wormPawn->doHorizontalAttack();
	}

	wormPawn->enableDamageArea();
}


void AnewWormCtrl::finishVerticalPreparation()
{
	toggleParticles( false );
	toggleGoToPlayer( false );
	wormPawn->toOverworldMode();

}
