// Fill out your copyright notice in the Description page of Project Settings.


#include "GetPlayerLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include <Kismet/GameplayStatics.h>



UGetPlayerLocation::UGetPlayerLocation()
{
	playerLocation.AddVectorFilter( this, GET_MEMBER_NAME_CHECKED( UGetPlayerLocation, playerLocation ) );
	
}

void UGetPlayerLocation::TickNode( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds )
{
	Super::TickNode( ownerComp, NodeMemory, DeltaSeconds );

	UBlackboardComponent* BlackboardComp = ownerComp.GetBlackboardComponent();
	APawn* player = UGameplayStatics::GetPlayerPawn( GetWorld(), 0 );
	if(player != nullptr)
	{
		BlackboardComp->SetValueAsVector( playerLocation.SelectedKeyName, player->GetActorLocation() );
	}
	

}


FString UGetPlayerLocation::GetStaticDescription() const
{
	return TEXT( "Obtiene la posicion del jugador" );
}