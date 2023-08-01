// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BaseEnemy.h"
#include <Scyther/Components/HealthComponent.h>
#include "newRabbitPawnBase.generated.h"


UENUM( BlueprintType )
enum class RabbitStates: uint8
{
	Idle,
	Moving,
	Hit,
	PreparingAttack,
	AttackReady,
	Attacking,
	AttackFinished
};

UENUM( BlueprintType )
enum class RabbitTargets: uint8
{
	ToPlayer,
	RunAway,
	waitForPlayer,
	ReturnHome,
	GoToLocation,
};

UCLASS()
class SCYTHER_API AnewRabbitPawnBase: public ABaseEnemy
{
	GENERATED_BODY()

		//------------------------//
		//   Components
		//------------------------//

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Components" )
		class USceneComponent* sceneComp;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Components" )
		class UFloatingPawnMovement* movementComp;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Components" )
		class USkeletalMeshComponent* rabbitSkeleton;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Components" )
		class USphereComponent* rabbitBoxColisionS;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Components" )
		class UBoxComponent* rabbitAttackColisionB;

	//------------------------//
	//   Debug
	//------------------------//

	bool debugHoppingText = true;

	//------------------------//
	//   Parameters
	//------------------------//

	// -- General -- //
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "NewRabbitPawn " )
		RabbitStates currentState = RabbitStates::Idle;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "NewRabbitPawn " )
		RabbitTargets currentTarget = RabbitTargets::ReturnHome;

	// -- Go To Player -- //
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | GoToPlayer" )
		int goToPlayerJumps = 3;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | GoToPlayer", meta = ( Units = "s" ) )
		float goToPlayerDelayAfterAllJumps = 2.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | GoToPlayer", meta = ( Units = "s" ) )
		float goToPlayerDelayAfterEveryJump = 0.5f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | GoToPlayer", meta = ( Units = "cm" ) )
		float goToPlayerMaxMoveSpeed = 100;

	// -- Return Home -- //
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Return home" )
		int returnHomeJumps = 6;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Return home", meta = ( Units = "s" ) )
		float returnHomeDelayAfterAllJumps = 1.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Return home", meta = ( Units = "s" ) )
		float returnHomeDelayAfterEveryJump = 0.25f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Return home", meta = ( Units = "cm" ) )
		float returnHomeMaxMoveSpeed = 500;

	// -- Run Away -- //
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Run Away" )
		int runAwayJumps = 6;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Run Away", meta = ( Units = "s" ) )
		float runAwayDelayAfterAllJumps = 1.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Run Away", meta = ( Units = "s" ) )
		float runAwayDelayAfterEveryJump = 0.25f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Run Away", meta = ( Units = "cm" ) )
		float runAwayMaxMoveSpeed = 500;

	// -- Hit -- //
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn ")
		class UAnimMontage* hitMontage;

	// -- Attack -- //
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Attack")
		float attackDamage = 10;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Attack")
		DamageModes damageMode = DamageModes::DASHEABLE;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Attack" )
		float attackKnockbackStrenght = 4000;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Attack" )
		float attackPreparationDelay = 1.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Attack" )
		float afterAttackDelay = 1.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NewRabbitPawn | Attack" )
		float attackMaxMoveSpeed = 1000;

private:
	// -- General -- //
	int jumpsToEnd = 0;

	TMap<RabbitTargets, int> jumpsMap;
	TMap<RabbitTargets, float> delayAfterAllJumpsMap;
	TMap<RabbitTargets, float> delayAfterEveryJumpMap;
	TMap<RabbitTargets, float> maxMoveSpeedMap;

	// -- Attack -- //
	bool isDamageActive = false;

	//------------------------//
	//   FUNCTIONS
	//------------------------//

public:
	AnewRabbitPawnBase();

	UFUNCTION( BlueprintCallable )
		void changeRabbitState(RabbitStates newState);

	// -- General -- //
	UFUNCTION( BlueprintCallable )
		void setNewTarget( RabbitTargets newTarget );


	// -- Hop/Movement -- //

	/// <summary>
	/// Change currentState to moving so the animation blueprint starts to hop and call the following flags. Also setups the initial parameters of the movement like the maxNumber of hops
	/// and stop any movement while the rabbit is preparing itself to hop.
	/// </summary>
	UFUNCTION( BlueprintCallable )
		void startHopping();

	/// <summary>
	/// Called at the end of the hopPreparation animation. The rabbit starts to move horizontaly at this stage. The speed depends on the currentTarget
	/// </summary>
	UFUNCTION( BlueprintCallable )
		void hopPreparationEnds();

	/// <summary>
	/// Called at the end of the hopAir animation. The rabbit stops to move.
	/// </summary>
	UFUNCTION( BlueprintCallable )
		void hopAirEnds();

	/// <summary>
	/// Called at the end of the hopLanding animation. Stops the rabbit from moving.
	/// </summary>
	UFUNCTION( BlueprintCallable )
		void hopLandEnds();

	/// <summary>
	/// Change rabbit state to Idle and stops any AI pathing movement
	/// </summary>
	UFUNCTION( BlueprintCallable )
		void rabbitToIdle();

	UFUNCTION( BlueprintCallable )
		float getDelayAfterAllJumps();
	UFUNCTION( BlueprintCallable )
		float getDelayAfterEveryJump();
	UFUNCTION( BlueprintCallable )
		int getMaxJumps();

	// -- Hit -- //
	void rabbitDoHitMontage();

	UFUNCTION()
		void rabbitWasHit( float oldHp, float newHp, DamageModes damageType );

	UFUNCTION( BlueprintCallable )
		void makeRabbitInmortalAndHeal();

	UFUNCTION( BlueprintCallable )
		void makeRabbitMortal();

	// -- Attack --//
	UFUNCTION( BlueprintCallable )
		void toggleBoxDamage(bool activate);

	UFUNCTION( BlueprintCallable )
		void startAttackPreparation();

	UFUNCTION( BlueprintCallable )
		void preparationFinished();

	UFUNCTION( BlueprintCallable )
		void doAttack();

	UFUNCTION( BlueprintCallable )
		void attackFinished();

	UFUNCTION()
		void OnBoxCollisionOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

protected:
	virtual void BeginPlay() override;

private:
	void stopAIMovement();


	UFUNCTION()
		void OnBoxHit( UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );

	void damagePlayer( AActor* player );
};