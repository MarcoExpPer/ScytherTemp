// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SelectTypeAttack.generated.h"

/**
 *	Este service se encarga de elegir el ataque que va a hacer el gusano en funcion de unos porcentajes puestos en
 *		AIWormController en funcion de si esta en modo luz o en modo oscuridad
 *	AVISO: AUNQUE ES FUNCIONAL EL SERVICE EL QUE ESTA USANDOSE PARA HACER LO MISMO PERO DE MANERA MAS CONTROLADA
 *		ES EL TASK SelectAttack
 */
UCLASS()
class SCYTHER_API USelectTypeAttack : public UBTService
{
	GENERATED_BODY()
		UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector AttackToSelect;


	USelectTypeAttack();

	virtual void TickNode( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds );

	virtual FString GetStaticDescription() const;
};
