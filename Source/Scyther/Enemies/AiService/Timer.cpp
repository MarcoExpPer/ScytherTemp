// Fill out your copyright notice in the Description page of Project Settings.


#include "Timer.h"
#include "BehaviorTree/BlackboardComponent.h"

UTimer::UTimer()
{
	Time.AddFloatFilter( this, GET_MEMBER_NAME_CHECKED( UTimer, Time ) );
}

void UTimer::TickNode(UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(ownerComp, NodeMemory, DeltaSeconds);
	UBlackboardComponent* PBlackBoardComponent = ownerComp.GetBlackboardComponent();
	Tiempo = PBlackBoardComponent->GetValueAsFloat( Time.SelectedKeyName ) + DeltaSeconds;
	PBlackBoardComponent->SetValueAsFloat( Time.SelectedKeyName, Tiempo);
}

FString UTimer::GetStaticDescription() const
{
	return TEXT("Es un contador de tiempo");
}

void UTimer::BeginPlay()
{
	Tiempo = 0.f;
}
