// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SelectAttack.generated.h"

/**
 *	Este task se encarga de elegir el ataque que va a hacer el gusano en funcion de unos porcentajes puestos en
 *		AIWormController en funcion de si esta en modo luz o en modo oscuridad
 */
UCLASS()
class SCYTHER_API USelectAttack : public UBTTaskNode
{
	GENERATED_BODY()
		UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector AttackToSelect;

	USelectAttack();

	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) override;

	virtual FString GetStaticDescription() const;
};
