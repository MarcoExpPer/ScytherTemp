// Fill out your copyright notice in the Description page of Project Settings.


#include "TeleportEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Scyther/Enemies/BaseEnemyCtrl.h"

UTeleportEnemy::UTeleportEnemy()
{
	targetLocation.AddVectorFilter( this, GET_MEMBER_NAME_CHECKED( UTeleportEnemy, targetLocation ) );
}

EBTNodeResult::Type UTeleportEnemy::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* NodeMemory)
{
	ABaseEnemyCtrl* ctrl = Cast<ABaseEnemyCtrl>( ownerComp.GetOwner() );

	if( ctrl != nullptr )
	{
		UBlackboardComponent* bb = ownerComp.GetBlackboardComponent();
		TArray<AActor*> SinUso;
		APawn* enemy = ctrl->GetPawn();
		FHitResult Vacio;
		FVector tp = bb->GetValueAsVector( targetLocation.SelectedKeyName );
		FVector tpFinal = FVector( tp.X, tp.Y, bb->GetValueAsVector( "EnemyPosition" ).Z +1.f);


		enemy->K2_SetActorLocation(tpFinal, true, Vacio, true);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

FString UTeleportEnemy::GetStaticDescription() const
{
	return TEXT( "Telettransporta al enemigo a la posicion del jugador" );
}
