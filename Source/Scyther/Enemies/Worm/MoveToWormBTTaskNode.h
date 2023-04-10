// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MoveToWormBTTaskNode.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API UMoveToWormBTTaskNode : public UBTTaskNode
{
	GENERATED_BODY()
	
		//Sellama al inciar este task
	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) override;

	//se llama constantemente cuando el execute task devuelve InProgress
	virtual void TickTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds ) override;

	//descripcion del task
	virtual FString GetStaticDescription() const override;
};
