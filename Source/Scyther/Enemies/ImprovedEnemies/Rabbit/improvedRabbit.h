// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Scyther/Enemies/BaseEnemy.h>
#include "improvedRabbit.generated.h"

UENUM( BlueprintType )
enum class rabbitAnimations: uint8
{
	NONE,
	RunningAway,
	GoingToPlayer,
	PreparingAttack,
	Attacking,
	HitAnimation,
};


UCLASS()
class SCYTHER_API AimprovedRabbit: public ABaseEnemy
{
	GENERATED_BODY()

public:
	//Components
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class USceneComponent* rabbitRoot;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class USkeletalMeshComponent* rabbitMesh;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class USphereComponent* rabbitHitbox;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UBoxComponent* rabbitDamageArea;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UFloatingPawnMovement* movementComp;

	class AScytherGameModeBase* gm;

	//Anims
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ANIMS" )
		class UAnimMontage* fullJumpMontage;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ANIMS" )
		class UAnimMontage* fastJumpMontage;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ANIMS" )
		class UAnimMontage* attackMontage;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ANIMS" )
		class UAnimMontage* attackPrepMontage;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ANIMS" )
		class UAnimMontage* hitMontage;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ANIMS" )
		rabbitAnimations currentAnimation = rabbitAnimations::NONE;

	UAnimInstance* animInstance;

	//RUN AWAY PARAMETERS
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ImprovedAI | RunAway" )
		float RA_speed = 800.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ImprovedAI | RunAway" )
		int RA_maxJumps = 3;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ImprovedAI | RunAway" )
		float RA_everyJumpDelay = 0.10f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ImprovedAI | RunAway" )
		float RA_allJumpsDelay = 2.f;

	//GOING TO PLAYER PARAMETERS
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ImprovedAI | GoingPlayer" )
		float GP_speed = 500.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ImprovedAI | GoingPlayer" )
		int GP_maxJumps = 5;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ImprovedAI | GoingPlayer" )
		float GP_everyJumpDelay = 0.2f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ImprovedAI | GoingPlayer" )
		float GP_allJumpsDelay = 1.f;

	//ATTACK PARAMETERS
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ImprovedAI | Attack" )
		float AT_speed = 1000.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ImprovedAI | Attack" )
		float AT_preparationExtraDelay = 0.5f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ImprovedAI | Attack" )
		float AT_finishAttackDelay = 2.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ImprovedAI | Attack" )
		float AT_range = 300;

	//Timers
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float singleJumpDelay = 0.5f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float restDelay = 4.f;

	FTimerHandle resetJumpTimerHandle;
	//Movement
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float movementSpeed = 500;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		int maxJumps = 3;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		int currentJumps = 0;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float rotationSpeed = 10.f;


	//Flocking
	float sphereOverlapRadius = 4000;
	float maxSpreadRange = 175;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float spreadGrade = 0.92f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float targetGrade = 0.75f;

	FVector spreadVector;
	FVector targetVector;

	float realMovementMultiplier = 1000;
	FVector realMovement;
	float timeSinceLastRotation = 0;

	//Attack
	bool canDoDamage = false;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		bool isPreparingAttack = false;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float attackKnockbackStrenght = 3000;

public:
	AimprovedRabbit();

protected:
	virtual void BeginPlay() override;
public:
	UFUNCTION( Blueprintcallable )
		void Tick( float DeltaTime );

	UFUNCTION( Blueprintcallable )
		void changeAnimation( rabbitAnimations newAnim );

	void flockToThisLocation( FVector newTargetLocation );

	UFUNCTION( Blueprintcallable )
		void enableMovement();

	UFUNCTION( Blueprintcallable )
		void disableMovement( bool fullStop );

	UFUNCTION( Blueprintcallable )
		void jumpAnimationFinished();
	UFUNCTION( Blueprintcallable )
		void hitAnimationFinished();
	UFUNCTION( Blueprintcallable )
		void preparationAnimFinished();
	UFUNCTION( Blueprintcallable )
		void attackAnimationFinished();

	void enableDamageArea();
	void damagePlayer( AActor* actor );

	UFUNCTION()
	void onAttackAreaOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
	
	
	UFUNCTION()
		void beHit( float oldHp, float newHp, DamageModes damageType );

	//UFUNCTION()
	//void onAttackAreaOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
};
