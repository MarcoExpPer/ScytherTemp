// Fill out your copyright notice in the Description page of Project Settings.


#include "GetIsInside.h"

#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "../Worm/AIWormCppController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"


UGetIsInside::UGetIsInside()
{
	PlayerInside.AddBoolFilter( this, GET_MEMBER_NAME_CHECKED( UGetIsInside, PlayerInside) );
	EnemyBug.AddBoolFilter( this, GET_MEMBER_NAME_CHECKED( UGetIsInside, EnemyBug ) );
}

void UGetIsInside::TickNode(UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(ownerComp, NodeMemory, DeltaSeconds);
	UBlackboardComponent* PBlackBoardComponent = ownerComp.GetBlackboardComponent();
	UNavigationSystemV1* NavSys = Cast< UNavigationSystemV1>( GetWorld()->GetNavigationSystem() );

	APawn* playerPawn = UGameplayStatics::GetPlayerPawn( GetWorld(), 0 );
	
	ABaseEnemyCtrl* ctrl = Cast<ABaseEnemyCtrl>( ownerComp.GetOwner() );
	if( IsValid(playerPawn))
	{
		FVector playerPosition = playerPawn->GetActorLocation();
		if( !NavSys || !playerPawn )
		{
			PBlackBoardComponent->SetValueAsBool( PlayerInside.SelectedKeyName, false );
			UE_LOG( LogTemp, Warning, TEXT( "false1" ) );
		}
		else
		{
			const ANavigationData* NavData = NavSys->GetNavDataForProps( playerPawn->GetNavAgentPropertiesRef() );
			if( !NavData )
			{
				PBlackBoardComponent->SetValueAsBool( PlayerInside.SelectedKeyName, false );
				UE_LOG( LogTemp, Warning, TEXT( "false2" ) );
			}
			else
			{

				FPathFindingQuery Query( playerPawn, *NavData, playerPawn->GetNavAgentLocation(), ctrl->GetPawn()->GetNavAgentLocation() );
				//la siguientes lineas hasta el if es codigo defensivo
				FVector normal1( 1000.f, 0.f, 0.f );
				FVector normal2( 0.f, 1000.f, 0.f );
				FPathFindingQuery Query1( playerPawn, *NavData, playerPawn->GetNavAgentLocation(), ctrl->GetPawn()->GetNavAgentLocation() + normal1 + normal2 );
				FPathFindingQuery Query2( playerPawn, *NavData, playerPawn->GetNavAgentLocation(), ctrl->GetPawn()->GetNavAgentLocation() + normal1 - normal2 );
				FPathFindingQuery Query3( playerPawn, *NavData, playerPawn->GetNavAgentLocation(), ctrl->GetPawn()->GetNavAgentLocation() - normal1 + normal2 );
				FPathFindingQuery Query4( playerPawn, *NavData, playerPawn->GetNavAgentLocation(), ctrl->GetPawn()->GetNavAgentLocation() - normal1 - normal2 );
				if( NavSys->TestPathSync( Query ) )
				{
					UE_LOG( LogTemp, Warning, TEXT( "true1" ) );
					PBlackBoardComponent->SetValueAsBool( PlayerInside.SelectedKeyName, true );
					PBlackBoardComponent->SetValueAsBool( EnemyBug.SelectedKeyName, false );

				}
				else if( ( NavSys->TestPathSync( Query1 ) || NavSys->TestPathSync( Query2 ) || NavSys->TestPathSync( Query3 ) || NavSys->TestPathSync( Query4 ) ) )
				{
					UE_LOG( LogTemp, Warning, TEXT( "true2" ) );
					PBlackBoardComponent->SetValueAsBool( EnemyBug.SelectedKeyName, true );
				}
				else
				{
					PBlackBoardComponent->SetValueAsBool( PlayerInside.SelectedKeyName, false );
					UE_LOG( LogTemp, Warning, TEXT( "false3" ) );
				}
			}
		}
	}
}

FString UGetIsInside::GetStaticDescription() const
{
	return TEXT( "Devuelve si el player esta dentro de la navmesh" );
}
