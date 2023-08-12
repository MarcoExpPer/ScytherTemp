// Fill out your copyright notice in the Description page of Project Settings.


#include "GetPlayerLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include <Kismet/GameplayStatics.h>
#include <Scyther/ScytherGameModeBase.h>
#include "Scyther/Player/ScytherPlayerPawn.h"

UGetPlayerLocation::UGetPlayerLocation()
{
	playerLocation.AddVectorFilter( this, GET_MEMBER_NAME_CHECKED( UGetPlayerLocation, playerLocation ) );
	
}

void UGetPlayerLocation::TickNode( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds )
{
	Super::TickNode( ownerComp, NodeMemory, DeltaSeconds );

	UBlackboardComponent* BlackboardComp = ownerComp.GetBlackboardComponent();


	AScytherGameModeBase* gm = Cast<AScytherGameModeBase>( UGameplayStatics::GetGameMode( GetWorld() ) );

	APawn* targetPawn = Cast<APawn>( UGameplayStatics::GetActorOfClass( GetWorld(), gm->aiPlayerBPClass ) );
	if( targetPawn == nullptr )
	{
		targetPawn = UGameplayStatics::GetPlayerPawn( GetWorld(), 0 );
	}

	if( targetPawn != nullptr)
	{
		BlackboardComp->SetValueAsVector( playerLocation.SelectedKeyName, targetPawn->GetActorLocation() );
	}
	

}


FString UGetPlayerLocation::GetStaticDescription() const
{
	return TEXT( "Obtiene la posicion del jugador" );
}