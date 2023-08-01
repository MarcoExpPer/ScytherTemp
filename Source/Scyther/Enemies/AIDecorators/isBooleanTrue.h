// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "isBooleanTrue.generated.h"



/**
 * 
 */
UCLASS()
class SCYTHER_API UisBooleanTrue : public UBTDecorator
{
	GENERATED_UCLASS_BODY()

	UPROPERTY( EditAnywhere, Category = Blackboard )
	FBlackboardKeySelector booleanParameter;

	

	virtual bool CalculateRawConditionValue( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) const override;

	FString GetStaticDescription() const;

};
