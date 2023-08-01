// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "LocationCloserThanADistance.generated.h"


namespace EBTBlackboardRestart
{
	enum Type
	{
		ValueChange		UMETA( DisplayName = "On Value Change", ToolTip = "Restart on every change of observed blackboard value" ),
		ResultChange	UMETA( DisplayName = "On Result Change", ToolTip = "Restart only when result of evaluated condition is changed" ),
	};
}

/**
 *
 */
UCLASS()
class SCYTHER_API ULocationCloserThanADistance: public UBTDecorator
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, Category = Blackboard )
	FBlackboardKeySelector locationToCheck;
	UPROPERTY( EditAnywhere, Category = Blackboard )
	FBlackboardKeySelector minimumDistance;

	UPROPERTY( Category = FlowControl, EditAnywhere )
		TEnumAsByte<EBTBlackboardRestart::Type> NotifyObserver;

	ULocationCloserThanADistance();

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

