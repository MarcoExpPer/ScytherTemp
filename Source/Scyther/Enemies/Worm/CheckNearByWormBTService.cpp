// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckNearByWormBTService.h"
#include "CheckNearByWormBTService.h"
#include "AIWormCppController.h"

void UCheckNearByWormBTService::TickNode( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds )
{
	Super::TickNode( OwnerComp, NodeMemory, DeltaSeconds );

	//obtenemos la referencia del AAIWormCppControlle
	AAIWormCppController* AIWormController = Cast<AAIWormCppController>( OwnerComp.GetOwner() );

	//llamamos a CheckNearByWorm porque tiene toda la logica para decirte si esta cerca o no y configurar la key en el blackboard
	AIWormController->CheckNearByWorm();
}
