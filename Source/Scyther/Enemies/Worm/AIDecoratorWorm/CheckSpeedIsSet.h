// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "CheckSpeedIsSet.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API UCheckSpeedIsSet : public UBTDecorator
{
	GENERATED_BODY()

		UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector SpeedToCheck;

	UCheckSpeedIsSet();

	virtual bool CalculateRawConditionValue( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) const override;

	virtual FString GetStaticDescription() const override;

};
