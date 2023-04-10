// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Timer.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API UTimer : public UBTService
{
	GENERATED_BODY()

		UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector Time;
	UTimer();
public:
	float Tiempo = 0;

	virtual void TickNode( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds );

	virtual FString GetStaticDescription() const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay();
};
