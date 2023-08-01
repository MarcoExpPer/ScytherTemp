// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MoveToTargetLocation.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API UMoveToTargetLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
	UPROPERTY( EditAnywhere, Category = Blackboard )
	FBlackboardKeySelector targetLocation;
	/*UPROPERTY( EditAnywhere )
		TSubclassOf<UCameraShake> MyShake;
	bool ValidShake = true;*/
	UMoveToTargetLocation();

	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory );

	virtual FString GetStaticDescription() const;
};
