// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Timer.generated.h"

/**
 *  Service que simplemente va contando el tiempo y lo guarda en una variable de la Blackboard.
 *	Aviso: este service no reinicia el valor si ha dejado de actuar el service, eso lo hace el decorator TimerCloser que es un decorador personalido de Worm.
 */
UCLASS()
class SCYTHER_API UTimer : public UBTService
{
	GENERATED_BODY()

		UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector Time;
	UTimer();
private:
	float Tiempo = 0;
public:

	virtual void TickNode( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory, float DeltaSeconds );

	virtual FString GetStaticDescription() const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay();
};
