// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BaseEnemyCtrl.h"
#include "newRabbitCtrlBase.generated.h"


///Forward declaration
enum class RabbitTargets: uint8;

UCLASS()
class SCYTHER_API AnewRabbitCtrlBase : public ABaseEnemyCtrl
{
	GENERATED_BODY()
	

		//------------------------//
		//   Parameters
		//------------------------//
public:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "NewRabbitPawnController " )
		class AnewRabbitPawnBase* rabbitPawn;

public:

	AnewRabbitCtrlBase();

	virtual void BeginPlay() override;

	virtual EPathFollowingRequestResult::Type moveToPosition( FVector targetPosition ) override;
};
