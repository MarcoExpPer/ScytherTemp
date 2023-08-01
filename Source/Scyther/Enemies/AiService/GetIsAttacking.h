// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GetIsAttacking.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API UGetIsAttacking : public UBTService
{
	GENERATED_BODY()
//Variables
public:

	UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector isAttacking;
//Functions
	UGetIsAttacking();


	virtual void TickNode( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds );

	virtual FString GetStaticDescription() const;
};
