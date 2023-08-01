// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CheckHorizontalAttack.generated.h"

/**
 *		Este task revisa si cuando el gusano ha decidido hacer el ataque horizontal si es posible realizar, y decide ya
 *	la posicion desde donde va a salir comprobando si tanto desde donde sale y donde caeria son lugares validos,
 *	y si no son validos vuelve a coger una posicion aleatoria a un rango(fijo) del player asi hasta llegar a 100 que entonces
 *	detectaria que ahi no puede hacer de forma correcta el ataque horizontal y cambiaria su tipo de ataque al vertical.
 */
UCLASS()
class SCYTHER_API UCheckHorizontalAttack : public UBTTaskNode
{
	GENERATED_BODY()
public:
	APawn* playerPawn;
	FVector targetPosition;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<FHitResult> OutHits;
	float RangeHorizontalAttack = 500.f;
	float TeleportRandX;
	float TeleportRandY;
	float TeleportX;
	float TeleportY;
	float TeleportZ;
	int cont = 0;

	UCheckHorizontalAttack();
	UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector locationToAttack;
	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) override;

	//se llama constantemente cuando el execute task devuelve InProgress
	virtual void TickTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds ) override;

	//descripcion del task
	virtual FString GetStaticDescription() const override;
};
