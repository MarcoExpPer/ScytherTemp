// Fill out your copyright notice in the Description page of Project Settings.


#include "TimerCloser.h"
#include "AIModule/Classes/AIController.h"
#include "../AIWormCppController.h"
#include "../../AiService/Timer.h"
#include "BehaviorTree/BlackboardComponent.h"

UTimerCloser::UTimerCloser()
{

	NodeName = "TimerCloser";
	TimerToCheck.AddFloatFilter( this, GET_MEMBER_NAME_CHECKED( UTimerCloser, TimerToCheck ) );
	
}

bool UTimerCloser::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	
	AAIWormCppController* ctrl = Cast<AAIWormCppController>( OwnerComp.GetOwner() );
	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
	float TimerToChecking = bb->GetValueAsFloat( TimerToCheck.SelectedKeyName );
	float TimeDelay = ctrl->GetTimeDelay();
	if(TimerToChecking >= TimeDelay )
	{
		bb->SetValueAsFloat( TimerToCheck.SelectedKeyName, 0.f );
		bb->SetValueAsBool( "isMoving?", true );
		return true;
	}
	return false;
}

FString UTimerCloser::GetStaticDescription() const
{
	return TEXT( "Comprueba si su tiempo es menor que otro definido" );
}
