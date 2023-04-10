// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectTypeAttack.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Scyther/Enemies/Worm/AIWormCppController.h"
#include "Scyther/Enemies/BaseEnemy.h"

USelectTypeAttack::USelectTypeAttack()
{
	AttackToSelect.AddBoolFilter( this, GET_MEMBER_NAME_CHECKED( USelectTypeAttack, AttackToSelect ) );
}

void USelectTypeAttack::TickNode(UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(ownerComp, NodeMemory, DeltaSeconds);
	AAIWormCppController* ctrl = Cast<AAIWormCppController>( ownerComp.GetOwner() );
	AWormPawn* Pawn = Cast<AWormPawn>(Cast<AAIController>( ownerComp.GetOwner() )->GetPawn());
	float random = FMath::RandRange( 0.0f, 1.0f );
	UBlackboardComponent* bb = ownerComp.GetBlackboardComponent();
	
	if( Pawn->isLightMode ==true)
	{//esta en modo luz
		float verticalLight = (ctrl->GetVerticalLight()/100);
		if( random < verticalLight || (verticalLight==1.0f && random==1.0f))
		{
			//haria el ataque vertical
			bb->SetValueAsBool( AttackToSelect.SelectedKeyName, true );
		}
		else
		{
			//haria el ataque horizontal
			bb->SetValueAsBool( AttackToSelect.SelectedKeyName, false );
		}
	}
	else if( Pawn->isLightMode == false )
	{//esta en modo oscuridad
		float verticalDark = ( ctrl->GetVerticalDark() / 100 );
		if( random < verticalDark || ( verticalDark == 1.0f && random == 1.0f ) )
		{
			//haria el ataque vertical
			bb->SetValueAsBool( AttackToSelect.SelectedKeyName, true );
		}
		else
		{
			//haria el ataque horizontal
			bb->SetValueAsBool( AttackToSelect.SelectedKeyName, false );
		}
	}
}

FString USelectTypeAttack::GetStaticDescription() const
{
	return TEXT("Selecciona uno de los dos ataques de forma aleatoria a partir de un porcentaje en funcion si es luz o sombra");
}
