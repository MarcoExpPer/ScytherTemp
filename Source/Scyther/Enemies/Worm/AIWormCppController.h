// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WormPawn.h"
#include "../BaseEnemyCtrl.h"
#include "AIWormCppController.generated.h"

/**
 *
 */
UCLASS()
class SCYTHER_API AAIWormCppController: public ABaseEnemyCtrl
{
	GENERATED_BODY()
		//Variables
public:

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "WormPawn" )
		AWormPawn* wormPawn;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Controller", meta = ( DisplayName = "worm pawn", ToolTip = "Pawn del gusano." ) )
		AWormPawn* worm;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Worm Settings" )
		float RangeHorizontalAttack=500.f;//no usar

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Worm Settings" )
		float TimeDelayToAttack;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Worm Settings" )
		float PercentageVerticalLight=50.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Worm Settings" )
		float PercentageVerticalDark=50.f;
	
protected:
private:
	//Functions
public:

	UFUNCTION( BlueprintCallable, Category = "Order" )
		void UpdateNextTargetPoint();
	UFUNCTION( BlueprintCallable, Category = "Order" )
		float GetTimeDelay();

	UFUNCTION( BlueprintCallable, Category = "Order" )
		float GetVerticalLight();
	UFUNCTION( BlueprintCallable, Category = "Order" )
		float GetVerticalDark();
	UFUNCTION( BlueprintCallable, Category = "Chase Player" )
		void CheckNearByWorm();
	UFUNCTION( BlueprintCallable, Category = "Chase Player" )
		EPathFollowingRequestResult::Type MoveToEnemy();


	virtual void BeginPlay() override;

	virtual void moveToPosition( FVector targetPosition );
	virtual bool DoBasicAttack( FVector targetPosition );
	
};
