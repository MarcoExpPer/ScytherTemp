// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TeleportEnemy.generated.h"

/**
 *
 */
UCLASS()
class SCYTHER_API UTeleportEnemy: public UBTTaskNode
{
	GENERATED_BODY()
	UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector targetLocation;

	UTeleportEnemy();

	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory );

	virtual FString GetStaticDescription() const;
};
