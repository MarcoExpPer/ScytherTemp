// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GetEnemyLocation.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API UGetEnemyLocation : public UBTService
{
	GENERATED_BODY()
		UGetEnemyLocation();


	UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector EnemyPosition;

	virtual void TickNode( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds );

	virtual FString GetStaticDescription() const;
};
