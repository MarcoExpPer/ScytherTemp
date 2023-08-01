// Fill out your copyright notice in the Description page of Project Settings.


#include "SetNewVelocity.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Scyther/Enemies/BaseEnemyCtrl.h"
#include "Scyther/Enemies/Worm/AIWormCppController.h"
#include "Scyther/Enemies/Worm/WormPawn.h"

USetNewVelocity::USetNewVelocity()
{
	NewVelocity.AddFloatFilter( this, GET_MEMBER_NAME_CHECKED( USetNewVelocity, NewVelocity ) );
}

EBTNodeResult::Type USetNewVelocity::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
	AAIWormCppController* ctrl = Cast<AAIWormCppController>( OwnerComp.GetOwner() );
	AWormPawn* worm = Cast<AWormPawn>( ctrl->GetPawn() );
	worm->FloatingPawnMovement->MaxSpeed = bb->GetValueAsFloat( NewVelocity.SelectedKeyName );
	return EBTNodeResult::Succeeded;

}
