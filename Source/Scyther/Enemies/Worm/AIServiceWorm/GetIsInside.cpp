// Fill out your copyright notice in the Description page of Project Settings.
// explicacion breve en el .h

#include "GetIsInside.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "../AIWormCppController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Scyther/Player/ScytherPlayerPawn.h"


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
	APawn* playerPawn = UGameplayStatics::GetPlayerPawn( GetWorld(), 0 );
	
	if( IsValid(playerPawn))
	{
		if( !NavSys || !playerPawn )
		{
			PBlackBoardComponent = ownerComp.GetBlackboardComponent();
			PBlackBoardComponent->SetValueAsBool( PlayerInside.SelectedKeyName, false );
			CanSet = true;
		}
		else
		{
			const ANavigationData* NavData = NavSys->GetNavDataForProps( playerPawn->GetNavAgentPropertiesRef() );
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
				FPathFindingQuery Query( playerPawn, *NavData, playerPawn->GetActorLocation(), ctrl->GetPawn()->GetNavAgentLocation() );
				//la siguientes lineas hasta el if es codigo defensivo
				FVector normal1( 500.f, 0.f, 0.f );
				FVector normal2( 0.f, 500.f, 0.f );
				FPathFindingQuery Query1( playerPawn, *NavData, playerPawn->GetActorLocation(), ctrl->GetPawn()->GetNavAgentLocation() + normal1 + normal2 );
				FPathFindingQuery Query2( playerPawn, *NavData, playerPawn->GetActorLocation(), ctrl->GetPawn()->GetNavAgentLocation() + normal1 - normal2 );
				FPathFindingQuery Query3( playerPawn, *NavData, playerPawn->GetActorLocation(), ctrl->GetPawn()->GetNavAgentLocation() - normal1 + normal2 );
				FPathFindingQuery Query4( playerPawn, *NavData, playerPawn->GetActorLocation(), ctrl->GetPawn()->GetNavAgentLocation() - normal1 - normal2 );
				FNavPathQueryDelegate prueba;
				if( NavSys->TestPathSync( Query)  )
				{
					if((Cast<AScytherPlayerPawn>(playerPawn)->state != MovementState::JUMPING && Cast<AScytherPlayerPawn>( playerPawn )->state != MovementState::FALLING && Cast<AScytherPlayerPawn>( playerPawn )->state != MovementState::INAPEX ))
					{ 
						CanSet = false;
						APawn* enemy = ctrl->GetPawn();
						Cast<AWormPawn>( enemy )->SetZDefault(playerPawn->GetActorLocation().Z-169.f);
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
					FPathFindingQuery Query5( playerPawn, *NavData, playerPawn->GetNavAgentLocation(), ctrl->GetPawn()->GetNavAgentLocation() );
					if( NavSys->TestPathSync( Query5 ) && Cast<AScytherPlayerPawn>( playerPawn )->state != MovementState::WALKING )
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
