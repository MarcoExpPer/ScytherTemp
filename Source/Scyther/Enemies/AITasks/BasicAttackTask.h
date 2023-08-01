// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BasicAttackTask.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API UBasicAttackTask : public UBTTaskNode
{
	GENERATED_BODY()
	UBasicAttackTask();

	UPROPERTY( EditAnywhere, Category = Blackboard )
	FBlackboardKeySelector locationToAttack;

	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory );

	virtual FString GetStaticDescription() const;
};
