// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToTargetLocation.h"
#include <Scyther/Enemies/BaseEnemyCtrl.h>
#include "Camera/PlayerCameraManager.h"
#include "BehaviorTree/BlackboardComponent.h"

UMoveToTargetLocation::UMoveToTargetLocation()
{
	targetLocation.AddVectorFilter( this, GET_MEMBER_NAME_CHECKED( UMoveToTargetLocation, targetLocation ) );
}

EBTNodeResult::Type UMoveToTargetLocation::ExecuteTask( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory )
{
	ABaseEnemyCtrl *ctrl = Cast<ABaseEnemyCtrl>( ownerComp.GetOwner() );

	if( ctrl != nullptr )
	{
		UBlackboardComponent* bb = ownerComp.GetBlackboardComponent();
		ctrl->moveToPosition( bb->GetValueAsVector( targetLocation.SelectedKeyName ) );
		/*if((ValidShake == true  ) )
		{
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayWorldCameraShake( GetWorld(), MyShake, FVector( 0, 0, 0 ), 10000.f, 20000.f, 1.0f );
		}*/
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

FString UMoveToTargetLocation::GetStaticDescription() const
{
	return TEXT("Llama a la funcion de movimiento del controlador");
}
