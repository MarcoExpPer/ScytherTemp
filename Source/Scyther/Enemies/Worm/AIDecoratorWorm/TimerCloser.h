// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "TimerCloser.generated.h"

/**
 * Este decorator comprueba si el tiempo que va calculando el service Timer es mayor o igual del asignado en el AIWormController y cuando es asi tambien resetea ese tiempo calculado en el timer
 *
 */


UCLASS()
class SCYTHER_API UTimerCloser : public UBTDecorator
{
	GENERATED_BODY()
public:
	UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector TimerToCheck;

	UTimerCloser();

	virtual bool CalculateRawConditionValue( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) const override;

	virtual FString GetStaticDescription() const override;

};
