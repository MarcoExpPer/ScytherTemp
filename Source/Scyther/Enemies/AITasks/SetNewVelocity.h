// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SetNewVelocity.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API USetNewVelocity : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector NewVelocity;

	USetNewVelocity();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
