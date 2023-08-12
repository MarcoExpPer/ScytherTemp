// Fill out your copyright notice in the Description page of Project Settings.


#include "AIWormCppController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../AIWaypointGeneral.h"
//#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "Runtime/Engine/Public/EngineUtils.h"
//#include "NavigationSystem.h"



void AAIWormCppController::BeginPlay()
{
	Super::BeginPlay();
	worm = Cast<AWormPawn>( this->GetPawn() );
	UBlackboardComponent* pBlackBoardComponent = BrainComponent->GetBlackboardComponent();
	pBlackBoardComponent->SetValueAsBool( "isMoving?", true );
	pBlackBoardComponent->SetValueAsFloat( "SpeedToGoNear", SpeedToGoNear );
	pBlackBoardComponent->SetValueAsFloat( "DefaultSpeed", worm->FloatingPawnMovement->MaxSpeed );
	NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached( EffectMoveToLocation, Cast<AWormPawn>( this->GetPawn() )->EffectsPlace, NAME_None, FVector( 0.f ), FRotator( 0.f ), EAttachLocation::Type::KeepRelativeOffset, true );
	
}
void AAIWormCppController::UpdateNextTargetPoint()
{
	//
	TArray<AAIWaypointGeneral*> aLista = wormPawn->TargetPoints;
	//Se obtiene la referencia que va desde el Controlador de IA al BlackBoardComponent
	UBlackboardComponent* pBlackBoardComponent = BrainComponent->GetBlackboardComponent();
	//Guarda en TargetPointOrder el valor que tiene el BlackBoard en el KEY TargetPointOrder
	//Este número nos indica el orden en el que el moñeco se va a mover por el nivel
	int32 iTargetPointOrder = pBlackBoardComponent->GetValueAsInt( "TargetPointOrder" );
	//Pilla el número del TargetPoints que hay en el nivel y cuando lo supera se reinicia al primero
	if( iTargetPointOrder >= aLista.Num() )
	{
		//Pone el valor de iTargetPointOrder a 0
		iTargetPointOrder = 0;
		pBlackBoardComponent->SetValueAsInt( "TargetPointOrder", iTargetPointOrder );
	}
	//Setteamos el TargetPointPosition con la posicion de ese TargetPoint en el nivel y detenemos el ciclo con el break;
	pBlackBoardComponent->SetValueAsVector( "TargetPointPosition", aLista[iTargetPointOrder]->GetActorLocation() );
	// Por último, incrementamso el valor de TargetPointOrder del Blackboard
	pBlackBoardComponent->SetValueAsInt( "TargetPointOrder", ( iTargetPointOrder + 1 ) );
}

float AAIWormCppController::GetTimeDelay()
{
	UBlackboardComponent* pBlackBoardComponent = BrainComponent->GetBlackboardComponent();
	if( pBlackBoardComponent->GetValueAsBool( "AttackType" ) == true )//si es true es el vertical
	{
		return TimeDelayToVerticalAttack;
	}
	else
	{
		return TimeDelayToHorizontalAttack;
	}
}

float AAIWormCppController::GetVerticalLight()
{
	return Cast<AWormPawn>(this->GetPawn())->PercentageVerticalLight;
}

float AAIWormCppController::GetVerticalDark()
{
	return Cast<AWormPawn>( this->GetPawn() )->PercentageVerticalDark;
}

void AAIWormCppController::PlayChaseSound(bool AttackType)
{
	if(AttackType == true)
	{
		UFMODBlueprintStatics::PlayEvent2D( GetWorld(), worm->VerticalAttackSound, true ); // 2D Sound
	}
	else
	{
		UFMODBlueprintStatics::PlayEvent2D( GetWorld(), worm->HorizontalAttackSound, true ); // 2D Sound
	}
}

EPathFollowingRequestResult::Type AAIWormCppController::moveToPosition( FVector targetPosition )
{
	if( worm != nullptr )
	{
		targetPosition.Z = worm->ZLocationDefault-60.f;

		
		//FVector	target = FVector( targetPosition.X, targetPosition.Y, worm->ZLocationDefault );

		return this->MoveToLocation( targetPosition, 1.0f, false, false );
		//GEngine->AddOnScreenDebugMessage( -1, 15.0f, FColor::Yellow, FString::Printf( TEXT( "Z para el worm cuando te sigue: %f" ), targetPosition.Z ) );

	}
	return EPathFollowingRequestResult::Failed;
}

bool AAIWormCppController::DoBasicAttack( FVector targetPosition )
{
	if( NiagaraComp->IsValidLowLevel() )
	{
		NiagaraComp->ActivateSystem( false );
	}
	
	UBlackboardComponent* BlackboardComponent = BrainComponent->GetBlackboardComponent();
	if( worm != nullptr )
	{
		bool AttackType = BlackboardComponent->GetValueAsBool( "AttackType" );
		BlackboardComponent->SetValueAsBool( "IsCheck?", false );
		if( AttackType == true )
		{//vertical
			StopMovement();
			BlackboardComponent->SetValueAsFloat( "Timer", 0.f );
			worm->VerticalAttack();
			return true;
		}
		else
		{//horizontal
			FVector playerPosition = BlackboardComponent->GetValueAsVector( "PositionHorizontalAttack" );
			FHitResult CollisionTeleport;

			StopMovement();
			//GEngine->AddOnScreenDebugMessage( -1, 15.0f, FColor::Yellow, FString::Printf( TEXT( "Z worm tp horizontal: %f" ), playerPosition.Z) );
			//worm->K2_SetActorLocation( playerPosition, false, CollisionTeleport, true );

			BlackboardComponent->SetValueAsFloat( "Timer", 0.f);
			BlackboardComponent->SetValueAsBool( "IsReadyToAttack", false );
			BlackboardComponent->SetValueAsBool( "DontCheck", false );
			worm->HorizontalAttack( BlackboardComponent->GetValueAsVector("RotationAttack"), BlackboardComponent->GetValueAsVector( "PositionHorizontalAttack" ) );
			
			return true;

		}
	}

	return false;
}


