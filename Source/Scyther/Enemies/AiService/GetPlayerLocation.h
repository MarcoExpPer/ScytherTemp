// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GetPlayerLocation.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API UGetPlayerLocation : public UBTService
{
	GENERATED_BODY()
		UGetPlayerLocation();

		UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector playerLocation;

		

		virtual void TickNode( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds );

		virtual FString GetStaticDescription() const;
};
