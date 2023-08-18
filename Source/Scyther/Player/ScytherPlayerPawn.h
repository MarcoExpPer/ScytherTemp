// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// 1- Includes
#include "fmod.hpp"
#include "FMODEvent.h"
#include "FMODBlueprintStatics.h"
#include <FMODAudioComponent.h>
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "ScytherPlayerPawn.generated.h"
// 2- Defines
// 3- Using

// 4- Clases y estructuras auxiliares
UENUM()
enum ZTargetModes
{
	ZT_None UMETA( DisplayName = "NONE" ),
	ZT_Simple UMETA( DisplayName = "SIMPLE" ),
	ZT_Advanced UMETA( DisplayName = "ADVANCED" ),
};

UENUM( BlueprintType, meta = ( DisplayName = "Player Movement State" ) )
enum class MovementState: uint8
{
	NONE UMETA ( DisplayName = "No movement" ),
	WALKING UMETA( DisplayName = "Caminando" ),
	JUMPING UMETA( DisplayName = "Saltando" ),
	INAPEX UMETA( DisplayName = "En el apex" ),
	FALLING  UMETA( DisplayName = "Cayendo" ),
	DASHING UMETA(DisplayName = "Dasheando"),
	ATTACKING UMETA(DisplayName = "Atacando"),
	PREPARING_DROP UMETA(DisplayName = "Preparando Drop"),
	VERTICAL_DROP UMETA(DisplayName = "Vertical Drop"),
	KNOCKBACK,
};

UCLASS()
/// <summary>
/// <para>
/// Stores the main protagonist class for Gloom Reaper.
/// </para>
/// <para>
/// The project makes use of the inherited blueprint class "BP_ScytherPlayerPawn"
/// </para>
/// </summary>
class SCYTHER_API AScytherPlayerPawn: public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AScytherPlayerPawn();

	//------------------------//
	//      COMPONENTS
	//------------------------//

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>Refers to the camera spring that allows the player
		/// to run into walls without the vision being affected</value></summary>
		class USpringArmComponent* springArm;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>Refers to the camera that follows the player. 
		/// It is not being used in the game at the moment.</value></summary>
		class UCameraComponent* playerCamera;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>Refers to the camera that follows the player. 
		/// It is not being used in the game at the moment.</value></summary>
		class USkeletalMeshComponent* mesh;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>Refers to the camera that follows the player. 
		/// It is not being used in the game at the moment.</value></summary>
		class UCapsuleComponent* capsule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		/// <summary><value>Refers to the scythe which contains the mesh</value></summary>
		class UStaticMeshComponent* scytheMesh;

	UPROPERTY( EditAnywhere, blueprintReadWrite )
		class UNiagaraComponent* dashNiagaraSystem;

	UPROPERTY( EditAnywhere, blueprintReadWrite )
		class UNiagaraComponent* dashAvailableNiagaraSystem;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>Refers to the collider used for the attack</value></summary>
		class UBoxComponent* scytheAttackCol;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>Refers to the collider used for vertical attack player collisions</value></summary>
		class UCapsuleComponent* verticalAttackCol;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>Refers to the collider used for the dark skill</value></summary>
		class USphereComponent* SphereSkillCollision;

	UPROPERTY( EditAnywhere, blueprintReadWrite )
		class UNiagaraComponent* niagaraSystem;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UStaticMeshComponent* entityShadow;	
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UStaticMeshComponent* circleRingShadow;

	//------------------------//
	//   PRIMITIVE COMPONENTS
	//------------------------//

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>Property where the character's health is stored.
		/// This includes regular health, and the logic of death, change events and god mode.</value></summary>
		class UHealthComponent* healthComponent;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>Property where the character's accumulated blood is stored.
		/// This includes is amount, its use and its god mode logic.</value></summary>
		class UBloodComponent* bloodComponent;

	UPROPERTY( EditAnywhere, BlueprintReadWrite ) 
		/// <summary><value>Perception behavior for the AI to recognise the presence of the character.</value></summary>
		class UAIPerceptionStimuliSourceComponent* AIPercetionComponent;

	UPROPERTY( EditAnywhere, BluePrintReadWrite )
		/// <summary><value>Z-targeting behavior for the character to be able to perform enemy focus.</value></summary>
		class UTargetingComponent* targetComponent;

	UPROPERTY( EditAnywhere, BluePrintReadWrite )
		/// <summary><value>Skill behavior for the character to throw any skill.</value></summary>
		class USkillComponent* skillComponent;

	UPROPERTY( EditAnywhere, BluePrintReadWrite )
		/// <summary><value>Movement behavior who controls the player and his collisions.</value></summary>
		class U3D_MovementComponent* movementComp;
	
	UPROPERTY( EditAnywhere, BluePrintReadWrite )
		/// <summary><value>Attack behavior for the character to do damage to the enemies.</value></summary>
		class UAttackComponent* attackComponent;	
	
	UPROPERTY( EditAnywhere, BluePrintReadWrite )
		/// <summary><value>Grapple hook behaviour for the character .</value></summary>
		class UGrappleHookComponent* grappleHookComponent;

	UPROPERTY( EditAnywhere, BluePrintReadWrite )
		/// <summary><value>Dash behaviour for the character .</value></summary>
		class UDashComponent* dashComponent;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>
		class UTransverseComponent* transverseComponent;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>
		class UEntityShadow* entityShadowComponent;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>
		class UScythePosController* scythePosComponent;

	//------------------------//
	//   FMOD
	//------------------------//

	//------------------------//
	//   RAY CASTS
	//------------------------//

	//------------------------//
	//  PARAMETERS
	//------------------------//
	UPROPERTY( EditAnywhere, BluePrintReadWrite, Category = "Design | DEBUG " )
		bool stateText = false;
	UPROPERTY( EditAnywhere, BluePrintReadWrite, Category = "Design | DEBUG " )
		bool playerLocationText = true;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
	MovementState state = MovementState::FALLING;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
	bool isGrounded;

	UPROPERTY( EditAnywhere, BluePrintReadWrite )
	bool isGodMode = false;

	//-- EXPERIENCE --/ /--deprecated--/
public:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Design|Experience" )
		float currentExperience = 0;

private:

	bool enemyTargeted = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent ) override;

	/// <summary>
	/// <para>
	/// Executes movement for the character on the horizontal axis.
	/// </para>
	/// <para>
	/// La precisión podrá ser 0 para generar números enteros.
	/// </para>
	/// </summary>
	/// <param name="Val">Input value for the horizontal axis.</param>
	/// <returns>
	/// This function returns no value.
	/// </returns>
	UFUNCTION( BlueprintCallable )
	void MoveRight( float Val );

	/// <summary>
	/// <para>
	/// Executes movement for the character on the vertical axis.
	/// </para>
	/// </summary>
	/// <param name="Val">Input value for the vertical axis.</param>
	/// <returns>
	/// This function returns no value.
	UFUNCTION( BlueprintCallable )
	void MoveForward( float Val );

	/// <summary>
	/// <para>
	/// Executes jump movement and animation for the character.
	/// </para>
	/// </summary>
	/// <param name="No params"></param>
	/// <returns>
	/// This function returns no value.
	/// </returns>
	UFUNCTION( BlueprintCallable )
	void OnJumpPressed();

	/// <summary>
	/// <para>
	/// Interrupts jump and executes falling movement and animation for the character.
	/// </para>
	/// </summary>
	/// <param name="No params"/>
	/// <returns>
	/// This function returns no value.
	/// </returns>
	UFUNCTION( BlueprintCallable )
	void OnJumpReleased();

	/// <summary>
	/// <para>
	/// Executes the attack animation and logic for the player.
	/// </para>
	/// <para>
	/// Not finished.
	/// </para>
	/// </summary>
	/// <param name="No params"/>
	/// <returns>
	/// This function returns no value.
	/// </returns>
	UFUNCTION( BlueprintCallable )
	void OnAttack();

	/// <summary>
	/// Execute the dash function of the dash component
	/// </summary>
	UFUNCTION( BlueprintCallable )
	void OnDash();

	/// <summary>
	/// <para>
	/// Executes the Z-targeting logic for the character, reacting to the current state of the targeting component.
	/// </para>
	/// </summary>
	/// <param name="No params"></param>
	/// <returns>
	/// This function returns no value.
	/// </returns>
	void TargetPressed();
	void TargetSwitchRight( float value );
	void TargetSwitchUp( float value );

	/// <summary>
	/// Enableds the skill a.
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void EnabledSkillA();
	/// <summary>
	/// Enableds the skill b.
	/// </summary>
	UFUNCTION( BlueprintCallable )
	void EnabledSkillB();

	/// <summary>
	/// Enableds the grapple hook.
	/// </summary>
	UFUNCTION( BlueprintCallable )
	void ActivateGrappleHook();

	/// <summary>
	/// Traverse on Spawn points on Left or Right
	// </summary>
	void MoveToTheNextPos( FTransform pos );
	void MoveToThePreviousPos( FTransform pos );

	UFUNCTION( BlueprintCallable )
	void ActivateGODMode();

	UFUNCTION(BlueprintCallable)
	void ChangeMovementState( MovementState newState, bool fixPlayerToGround = false);

	UFUNCTION( BlueprintCallable )
	void ApplyKnockback(FVector knockbackStrength, float knockbackDuration);

private:

	void RotateTowardsZTarget();

	void InitComponents();
	void InitPrimitiveComponents();

//------------------------//
// AUXILIAR
//------------------------//
public:
	/// <summary>
	/// Returns a string with the name of a movementState shared as parameter
	/// </summary>
	UFUNCTION( BlueprintCallable )
	FString GetStateAsName( MovementState EnumValue );

	/// <summary>
	/// Executes the logic of using the blood available and using it to recover healthr
	/// </summary>
	UFUNCTION( BlueprintCallable )
	void OnBloodRecover();
};
