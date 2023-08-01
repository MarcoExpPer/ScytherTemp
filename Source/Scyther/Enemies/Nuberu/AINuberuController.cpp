// Fill out your copyright notice in the Description page of Project Settings.


#include "AINuberuController.h"

#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Scyther/Enemies/AIWaypointGeneral.h"


void AAINuberuController::BeginPlay()
{
	Super::BeginPlay();
	NuberuPawn = Cast<ANuberuPawn>(this->GetPawn());
	UBlackboardComponent* pBlackBoardComponent = BrainComponent->GetBlackboardComponent();
	pBlackBoardComponent->SetValueAsFloat( "DistanceToDetect", DistanceToDetect );
	pBlackBoardComponent->SetValueAsFloat( "DistanceToAttack", DistanceToAttack );
	pBlackBoardComponent->SetValueAsFloat( "DistanceToGoAway", DistanceToGoAway );
}
void AAINuberuController::UpdateNextTargetPoint()
{
	TArray<AAIWaypointGeneral*> aLista = NuberuPawn->TargetPoints;
	UBlackboardComponent* pBlackBoardComponent = BrainComponent->GetBlackboardComponent();
	int32 iTargetPointOrder = pBlackBoardComponent->GetValueAsInt("TargetPointOrder");

	if( iTargetPointOrder >= aLista.Num() )
	{
		//Pone el valor de iTargetPointOrder a 0
		iTargetPointOrder = 0;
		pBlackBoardComponent->SetValueAsInt( "TargetPointOrder", iTargetPointOrder );
	}
	pBlackBoardComponent->SetValueAsVector( "TargetPointPosition", aLista[iTargetPointOrder]->GetActorLocation() );
	pBlackBoardComponent->SetValueAsInt( "TargetPointOrder", ( iTargetPointOrder + 1 ) );

}

EPathFollowingRequestResult::Type AAINuberuController::moveToPosition( FVector targetPosition )
{
	if( NuberuPawn != nullptr )
	{
		return this->MoveToLocation( targetPosition, 1.0f, false, true );
	}
	return EPathFollowingRequestResult::Failed;
}

bool AAINuberuController::DoBasicAttack( FVector targetPosition )
{
	UBlackboardComponent* BlackboardComponent = BrainComponent->GetBlackboardComponent();
	if((NuberuPawn != nullptr))
	{
		StopMovement();
		NuberuPawn->RayAttack(targetPosition);
		return true;
	}else
	{
		return false;
	}
}