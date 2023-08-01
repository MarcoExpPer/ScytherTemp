// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BaseEnemyCtrl.h"
#include "AIController.h"
#include "NuberuPawn.h"
#include "AINuberuController.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API AAINuberuController : public ABaseEnemyCtrl
{
	GENERATED_BODY()
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Nuberu" )
		ANuberuPawn* NuberuPawn;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Nuberu" )
		float DistanceToDetect = 2500.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Nuberu" )
		float DistanceToAttack = 1500.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Nuberu" )
		float DistanceToGoAway = 700.f;
	//Functions
public:
	UFUNCTION( BlueprintCallable, Category = "Order" )
		void UpdateNextTargetPoint();
	virtual EPathFollowingRequestResult::Type moveToPosition( FVector targetPosition );
	virtual bool DoBasicAttack( FVector targetPosition );
	virtual void BeginPlay() override;
};


