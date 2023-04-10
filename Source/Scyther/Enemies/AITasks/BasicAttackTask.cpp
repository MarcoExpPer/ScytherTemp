// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicAttackTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <Scyther/Enemies/BaseEnemyCtrl.h>


UBasicAttackTask::UBasicAttackTask()
{
	locationToAttack.AddVectorFilter( this, GET_MEMBER_NAME_CHECKED( UBasicAttackTask, locationToAttack ) );
}


EBTNodeResult::Type UBasicAttackTask::ExecuteTask( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory )
{
	ABaseEnemyCtrl* ctrl = Cast<ABaseEnemyCtrl>( ownerComp.GetOwner() );

	if( ctrl != nullptr )
	{
		UBlackboardComponent* bb = ownerComp.GetBlackboardComponent();

		bool resul = ctrl->DoBasicAttack(bb->GetValueAsVector( locationToAttack .SelectedKeyName));

		return resul ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}

FString UBasicAttackTask::GetStaticDescription() const
{
	return TEXT( "Llama a la funcion de ataque basico del controlador" );
}