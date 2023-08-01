// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckSpeedIsSet.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Scyther/Enemies/Worm/AIWormCppController.h"
#include "Scyther/Enemies/Worm/WormPawn.h"

UCheckSpeedIsSet::UCheckSpeedIsSet()
{
	SpeedToCheck.AddFloatFilter( this, GET_MEMBER_NAME_CHECKED( UCheckSpeedIsSet, SpeedToCheck ) );
}

bool UCheckSpeedIsSet::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
	AAIWormCppController* ctrl = Cast<AAIWormCppController>( OwnerComp.GetOwner() );
	AWormPawn* worm = Cast<AWormPawn>(ctrl->GetPawn());
	float MaxSpeed = worm->FloatingPawnMovement->MaxSpeed;
	if( bb->GetValueAsFloat( SpeedToCheck.SelectedKeyName ) != MaxSpeed)
	{
		return true;
	}
	
	return false;
}

FString UCheckSpeedIsSet::GetStaticDescription() const
{
	return TEXT("Compara el max speed del gusano con otro valor para saber si es la misma velocidad");
}
