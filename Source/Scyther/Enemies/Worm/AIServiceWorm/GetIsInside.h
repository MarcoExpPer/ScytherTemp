// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GetIsInside.generated.h"


/**
 *
 * Este service te dice si el player esta dentro de la navmesh del enemy, devolviendo true cuando esta dentro y false cuando no
 *
 * -Variables:
 *		Playerinside: aqui se seleccionara la variable de la Blackboard en la que ira actualizando si el jugador esta dentro o fuera de la nacmesh
 *		EnemyBug: sirve para seleccionar una variable de la blackboard que actualizara solo en casode que por algun motivo desconocido se saliera el enemy de la navmesh y dejara de realizar sus acciones
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

	bool CanSet = true;
		UGetIsInside();

		virtual void TickNode( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds );

	virtual FString GetStaticDescription() const ;
};
