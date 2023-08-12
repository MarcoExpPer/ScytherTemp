// Fill out your copyright notice in the Description page of Project Settings.
// explicacion breve en el .h

#include "GetIsInside.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "../AIWormCppController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Scyther/Player/ScytherPlayerPawn.h"
#include <Scyther/ScytherGameModeBase.h>

UGetIsInside::UGetIsInside()
{
	PlayerInside.AddBoolFilter( this, GET_MEMBER_NAME_CHECKED( UGetIsInside, PlayerInside) );
	EnemyBug.AddBoolFilter( this, GET_MEMBER_NAME_CHECKED( UGetIsInside, EnemyBug ) );
}

void UGetIsInside::TickNode(UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(ownerComp, NodeMemory, DeltaSeconds);
	UBlackboardComponent* PBlackBoardComponent;
	UNavigationSystemV1* NavSys = Cast< UNavigationSystemV1>( GetWorld()->GetNavigationSystem() );

	AScytherGameModeBase* gm = Cast<AScytherGameModeBase>( UGameplayStatics::GetGameMode( GetWorld() ) );
	
	APawn* targetPawn = Cast<APawn>( UGameplayStatics::GetActorOfClass( GetWorld(), gm->aiPlayerBPClass ) );
	if( targetPawn == nullptr )
	{
		targetPawn = UGameplayStatics::GetPlayerPawn( GetWorld(), 0 );
	}
	
	if( IsValid( targetPawn ))
	{
		if( !NavSys || !targetPawn )
		{
			PBlackBoardComponent = ownerComp.GetBlackboardComponent();
			PBlackBoardComponent->SetValueAsBool( PlayerInside.SelectedKeyName, false );
			CanSet = true;
		}
		else
		{
			const ANavigationData* NavData = NavSys->GetNavDataForProps( targetPawn->GetNavAgentPropertiesRef() );
			if( !NavData )
			{
				PBlackBoardComponent = ownerComp.GetBlackboardComponent();
				PBlackBoardComponent->SetValueAsBool( PlayerInside.SelectedKeyName, false );
				CanSet = true;
			}
			else
			{
				ABaseEnemyCtrl* ctrl = Cast<ABaseEnemyCtrl>( ownerComp.GetOwner() );
				
				//Query para saber si a traves de la posicion del player esta dentro o fuera de la navmesh
				FPathFindingQuery Query( targetPawn, *NavData, targetPawn->GetActorLocation(), ctrl->GetPawn()->GetNavAgentLocation() );
				//la siguientes lineas hasta el if es codigo defensivo
				FVector normal1( 500.f, 0.f, 0.f );
				FVector normal2( 0.f, 500.f, 0.f );
				FPathFindingQuery Query1( targetPawn, *NavData, targetPawn->GetActorLocation(), ctrl->GetPawn()->GetNavAgentLocation() + normal1 + normal2 );
				FPathFindingQuery Query2( targetPawn, *NavData, targetPawn->GetActorLocation(), ctrl->GetPawn()->GetNavAgentLocation() + normal1 - normal2 );
				FPathFindingQuery Query3( targetPawn, *NavData, targetPawn->GetActorLocation(), ctrl->GetPawn()->GetNavAgentLocation() - normal1 + normal2 );
				FPathFindingQuery Query4( targetPawn, *NavData, targetPawn->GetActorLocation(), ctrl->GetPawn()->GetNavAgentLocation() - normal1 - normal2 );
				FNavPathQueryDelegate prueba;
				if( NavSys->TestPathSync( Query)  )
				{
					if((Cast<AScytherPlayerPawn>( targetPawn )->state != MovementState::JUMPING && Cast<AScytherPlayerPawn>( targetPawn )->state != MovementState::FALLING && Cast<AScytherPlayerPawn>( targetPawn )->state != MovementState::INAPEX ))
					{ 
						CanSet = false;
						APawn* enemy = ctrl->GetPawn();
						Cast<AWormPawn>( enemy )->SetZDefault( targetPawn->GetActorLocation().Z-169.f);
					}
					PBlackBoardComponent = ctrl->GetBlackboardComponent();
					//PBlackBoardComponent = ownerComp.GetBlackboardComponent();
					PBlackBoardComponent->SetValueAsBool( PlayerInside.SelectedKeyName, true );
					PBlackBoardComponent->SetValueAsBool( EnemyBug.SelectedKeyName, false );
				}
				else if( ( NavSys->TestPathSync( Query1 ) || NavSys->TestPathSync( Query2 ) || NavSys->TestPathSync( Query3 ) || NavSys->TestPathSync( Query4 ) ) )
				{
					PBlackBoardComponent = ctrl->GetBlackboardComponent();
					PBlackBoardComponent->SetValueAsBool( PlayerInside.SelectedKeyName, false );
					PBlackBoardComponent->SetValueAsBool( EnemyBug.SelectedKeyName, true );
					PBlackBoardComponent->SetValueAsFloat( "Timer", 0.f );
					PBlackBoardComponent->SetValueAsBool( "IsCheck?", false );
					PBlackBoardComponent->SetValueAsBool( "IsMoving?", true );
					PBlackBoardComponent->SetValueAsBool( "IsReadyToAttack", false );
					CanSet = true;
				}
				else
				{
					FPathFindingQuery Query5( targetPawn, *NavData, targetPawn->GetNavAgentLocation(), ctrl->GetPawn()->GetNavAgentLocation() );
					if( NavSys->TestPathSync( Query5 ) && Cast<AScytherPlayerPawn>( targetPawn )->state != MovementState::WALKING )
					{
						PBlackBoardComponent = ctrl->GetBlackboardComponent();
						PBlackBoardComponent->SetValueAsBool( PlayerInside.SelectedKeyName, true );
						PBlackBoardComponent->SetValueAsBool( EnemyBug.SelectedKeyName, false );
					}
					else
					{
						PBlackBoardComponent = ctrl->GetBlackboardComponent();
						PBlackBoardComponent->SetValueAsBool( PlayerInside.SelectedKeyName, false );
						CanSet = true;
					}
					
				}
			}
		}
	}
}

FString UGetIsInside::GetStaticDescription() const
{
	return TEXT( "Devuelve si el player esta dentro de la navmesh" );
}
