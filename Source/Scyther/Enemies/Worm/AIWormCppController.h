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
		float SpeedMovementHorizontalAttack= 1000.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Worm Settings" )
		float TimeDelayToVerticalAttack;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Worm Settings" )
		float TimeDelayToHorizontalAttack;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Worm Settings " )
		float WaitAfterAttack = 2.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Worm Settings " )
		float SpeedToGoNear = 2000.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Worm Settings " )
		float RangeHorizontalAttack = 500.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Components" )
		UNiagaraSystem* EffectMoveToLocation;
		UNiagaraComponent* NiagaraComp;

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
	UFUNCTION( BlueprintCallable, Category = "FMOD" )
		void PlayChaseSound( bool AttackType );
	virtual void BeginPlay() override;

	virtual EPathFollowingRequestResult::Type moveToPosition( FVector targetPosition );
	virtual bool DoBasicAttack( FVector targetPosition );
	
	
};
