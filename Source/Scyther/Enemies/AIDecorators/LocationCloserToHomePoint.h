// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "LocationCloserThanADistance.h"
#include "LocationCloserToHomePoint.generated.h"



/**
 *
 */
UCLASS()
class SCYTHER_API ULocationCloserToHomePoint: public UBTDecorator
{
	GENERATED_BODY()


public:
	UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector locationToCheck;
	UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector minimumDistance;
	UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector homeLocation;

	UPROPERTY( Category = FlowControl, EditAnywhere )
		TEnumAsByte<EBTBlackboardRestart::Type> NotifyObserver;

	ULocationCloserToHomePoint();

	virtual bool CalculateRawConditionValue( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) const override;

	virtual FString GetStaticDescription() const override;

protected:

	/** initialize any asset related data */
	virtual void InitializeFromAsset( UBehaviorTree& Asset ) override;

	/** notify about change in blackboard keys */
	virtual EBlackboardNotificationResult OnBlackboardKeyValueChange( const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID );

	/** called when execution flow controller becomes active */
	virtual void OnBecomeRelevant( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) override;

	/** called when execution flow controller becomes inactive */
	virtual void OnCeaseRelevant( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) override;
};

