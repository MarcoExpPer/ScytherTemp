// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "./BaseEnemy.h"
#include "newWorm.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API AnewWorm : public ABaseEnemy
{
	GENERATED_BODY()
	

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite)
		class USceneComponent* wormRoot;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class USkeletalMeshComponent* wormMesh;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class USceneComponent* wormMeshLocation;

	UPROPERTY( EditAnywhere, BlueprintReadWrite)
		class UFloatingPawnMovement* movementComp;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "VFX" )
		class UNiagaraSystem* stonesVFX;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ANIMS")
		class UAnimMontage* verticalAttackMontage;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ANIMS" )
		class UAnimMontage* horizontalAttackMontage;

	bool isInAttackAnimation = false;
private:
	bool isDamageAreaActive = false;

	UAnimInstance* animInstance;

protected:
	virtual void BeginPlay() override;

public:
	AnewWorm();

	UFUNCTION(Blueprintcallable)
	void toUndergroundMode();
	UFUNCTION( Blueprintcallable )
	void toOverworldMode();

	UFUNCTION( Blueprintcallable )
	void enableDamageArea();
	UFUNCTION( Blueprintcallable )
	void disableDamageArea();

	UFUNCTION( Blueprintcallable )
	void doVerticalAttack();
	UFUNCTION( Blueprintcallable )
	void doHorizontalAttack();

	UFUNCTION()
	void OnMeshOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

private:
	void checkComponent( UPrimitiveComponent* compToCheck );
};
