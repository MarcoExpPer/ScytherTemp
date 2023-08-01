// Fill out your copyright notice in the Description page of Project Settings.


#include "UpdateWaypoint.h"

#include "Scyther/Enemies/Nuberu/AINuberuController.h"

EBTNodeResult::Type UUpdateWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAINuberuController* AIEnemyController = Cast<AAINuberuController>( OwnerComp.GetOwner() );
	AIEnemyController->UpdateNextTargetPoint();
	return EBTNodeResult::Succeeded;
}

FString UUpdateWaypoint::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}
