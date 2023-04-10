// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToWormBTTaskNode.h"
#include "AIWormCppController.h"

EBTNodeResult::Type UMoveToWormBTTaskNode::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{

	//si no pones esto a true no hace el TickTask nunca
	bNotifyTick = 1;

	return EBTNodeResult::InProgress;
}

void UMoveToWormBTTaskNode::TickTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds )
{

	
	AAIWormCppController* AIWormController = Cast<AAIWormCppController>( OwnerComp.GetOwner() );


	//se llama a MoveToEnemy y guardamos su resultado
	EPathFollowingRequestResult::Type MoveToActorResult = AIWormController->MoveToEnemy();


	//si ya llega al objetivo se acaba la ejecuion con FinishLatentTask
	if( MoveToActorResult == EPathFollowingRequestResult::AlreadyAtGoal )
	{
		
		FinishLatentTask( OwnerComp, EBTNodeResult::Succeeded );
	}
}

FString UMoveToWormBTTaskNode::GetStaticDescription() const
{
	return TEXT( "Persigue al personaje principal" );
}
