// Fill out your copyright notice in the Description page of Project Settings.


#include "GetEnemyLocation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include <Kismet/GameplayStatics.h>
#include "Scyther/Enemies/Worm/AIWormCppController.h"

UGetEnemyLocation::UGetEnemyLocation()
{
	EnemyPosition.AddVectorFilter( this, GET_MEMBER_NAME_CHECKED( UGetEnemyLocation, EnemyPosition ) );
}


void UGetEnemyLocation::TickNode( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds )
{
	Super::TickNode( ownerComp, NodeMemory, DeltaSeconds );
	UBlackboardComponent* BlackboardComp = ownerComp.GetBlackboardComponent();
	AWormPawn* Pawn = Cast<AWormPawn>( Cast<AAIController>( ownerComp.GetOwner() )->GetPawn() );
	BlackboardComp->SetValueAsVector( EnemyPosition.SelectedKeyName, Pawn->GetActorLocation() );

}


FString UGetEnemyLocation::GetStaticDescription() const
{
	return TEXT( "Obtiene la posicion del enemigo" );
}