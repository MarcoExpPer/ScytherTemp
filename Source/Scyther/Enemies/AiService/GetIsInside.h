// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Kismet/GameplayStatics.h"

#include "GetIsInside.generated.h"


/**
 * 
 */
UCLASS()
class SCYTHER_API UGetIsInside : public UBTService
{
	GENERATED_BODY()
	
		UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector PlayerInside;


	UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector EnemyBug;


		UGetIsInside();

		virtual void TickNode( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds );

	virtual FString GetStaticDescription() const ;
};
