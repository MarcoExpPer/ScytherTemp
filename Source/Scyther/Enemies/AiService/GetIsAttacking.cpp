// Fill out your copyright notice in the Description page of Project Settings.


#include "GetIsAttacking.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Scyther/Enemies/BaseEnemyCtrl.h>


UGetIsAttacking::UGetIsAttacking()
{
	isAttacking.AddBoolFilter( this, GET_MEMBER_NAME_CHECKED( UGetIsAttacking, isAttacking ) );
}

void UGetIsAttacking::TickNode( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds )
{
	Super::TickNode( ownerComp, NodeMemory, DeltaSeconds );

	ABaseEnemyCtrl* ctrl = Cast<ABaseEnemyCtrl>( ownerComp.GetOwner() );

	if( ctrl != nullptr )
	{
		UBlackboardComponent* bb = ownerComp.GetBlackboardComponent();

		if( ctrl->enemyPawn != nullptr ){ 
			bb->SetValueAsBool( isAttacking.SelectedKeyName, ctrl->enemyPawn->isAttacking );
		}
	}
}


FString UGetIsAttacking::GetStaticDescription() const
{
	return TEXT( "Obtiene la variable isAttacking del pawn" );
}