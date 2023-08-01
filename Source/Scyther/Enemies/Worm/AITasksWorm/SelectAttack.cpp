// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectAttack.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Scyther/Enemies/Worm/AIWormCppController.h"
#include "Scyther/Enemies/Worm/WormPawn.h"

USelectAttack::USelectAttack()
{
	AttackToSelect.AddBoolFilter( this, GET_MEMBER_NAME_CHECKED( USelectAttack, AttackToSelect ) );
}

EBTNodeResult::Type USelectAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIWormCppController* ctrl = Cast<AAIWormCppController>( OwnerComp.GetOwner() );
	AWormPawn* Pawn = Cast<AWormPawn>( Cast<AAIController>( OwnerComp.GetOwner() )->GetPawn() );
	float random = FMath::RandRange( 0.0f, 1.0f );
	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
	if( Pawn != nullptr )
	{
		if( Pawn->isLightMode == true )
		{//esta en modo luz
			float verticalLight = ( ctrl->GetVerticalLight() / 100 );
			if( random < verticalLight || ( verticalLight == 1.0f && random == 1.0f ) )
			{
				//haria el ataque vertical
				bb->SetValueAsBool( AttackToSelect.SelectedKeyName, true );
				bb->SetValueAsBool( "IsCheck?", true );
				//bb->SetValueAsObject("ShakeWorm", Nombre del audio para vertical);
				return EBTNodeResult::Succeeded;
			}
			else
			{
				//haria el ataque horizontal
				bb->SetValueAsBool( AttackToSelect.SelectedKeyName, false );
				bb->SetValueAsBool( "IsCheck?", true );
				//bb->SetValueAsObject("ShakeWorm", Nombre del audio para horizontal);
				return EBTNodeResult::Succeeded;
			}
		}
		else if( Pawn->isLightMode == false )
		{//esta en modo oscuridad
			float verticalDark = ( ctrl->GetVerticalDark() / 100 );
			if( random < verticalDark || ( verticalDark == 1.0f && random == 1.0f ) )
			{
				//haria el ataque vertical
				bb->SetValueAsBool( AttackToSelect.SelectedKeyName, true );
				bb->SetValueAsBool( "IsCheck?", true );
				//bb->SetValueAsObject("ShakeWorm", Nombre del audio para vertical);
				return EBTNodeResult::Succeeded;
			}
			else
			{
				//haria el ataque horizontal
				bb->SetValueAsBool( AttackToSelect.SelectedKeyName, false );
				bb->SetValueAsBool( "IsCheck?", true );
				//bb->SetValueAsObject("ShakeWorm", Nombre del audio para horizontal);
				return EBTNodeResult::Succeeded;
			}
		}
		else
		{
			return EBTNodeResult::Failed;
		}
	}
	else
	{
		return EBTNodeResult::Failed;
	}
}

FString USelectAttack::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}
