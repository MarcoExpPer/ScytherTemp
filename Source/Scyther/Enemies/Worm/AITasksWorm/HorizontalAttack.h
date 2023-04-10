// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "HorizontalAttack.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API UHorizontalAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	FVector targetPosition;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<FHitResult> OutHits;
	float RangeHorizontalAttack = 500.f;
	float TeleportRandX;
	float TeleportRandY;
	float TeleportX;
	float TeleportY;
	float TeleportZ;
	bool result = false;
	bool Invalid = false;
	bool valid = false;
	int cont = 0;

	UHorizontalAttack();
	UPROPERTY( EditAnywhere, Category = Blackboard )
	FBlackboardKeySelector locationToAttack;
	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) override;

	//se llama constantemente cuando el execute task devuelve InProgress
	virtual void TickTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds ) override;

	//descripcion del task
	virtual FString GetStaticDescription() const override;
};
