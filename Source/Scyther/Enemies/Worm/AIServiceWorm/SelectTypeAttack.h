// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SelectTypeAttack.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API USelectTypeAttack : public UBTService
{
	GENERATED_BODY()
		UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector AttackToSelect;


	USelectTypeAttack();

	virtual void TickNode( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds );

	virtual FString GetStaticDescription() const;
};
