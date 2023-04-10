// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GetRunAwayLocationFromLocation.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API UGetRunAwayLocationFromLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
	UPROPERTY( EditAnywhere, Category = Blackboard )
	FBlackboardKeySelector runAwayFromLocation;

	UPROPERTY( EditAnywhere, Category = Blackboard )
	FBlackboardKeySelector calculatedLocation;

	UGetRunAwayLocationFromLocation();

	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory );

	virtual FString GetStaticDescription() const override;
};
