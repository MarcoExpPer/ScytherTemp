// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// 1- Includes
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
	WALKING UMETA( DisplayName = "Caminando" ),
	JUMPING UMETA( DisplayName = "Saltando" ),
	INAPEX UMETA( DisplayName = "En el apex" ),
	FALLING  UMETA( DisplayName = "Cayendo" ),
	DASHING UMETA(DisplayName = "Dasheando"),
	ATTACKING UMETA(DisplayName = "Atacando")
};

UCLASS()
/// <summary>
/// <para>
/// Stores the main protagonist class for Gloom Reaper.
/// </para>
/// <para>
/// The project makes use of the inherited blueprint class "pruebaplayer"
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
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>Refers to the collider used for the attack</value></summary>
		class UBoxComponent* scytheAttackCol;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>Refers to the collider used for the skills</value></summary>
		class USphereComponent* SphereSkillCollision;

	//------------------------//
	//   PRIMITIVE COMPONENTS
	//------------------------//

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>Propuerty where all the types of the character's health are stored.
		/// This includes regular health, light, dark and experience.</value></summary>
		class UHealthComponent* healthComponent;

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
		/// <summary><value>Grapple hook behaviour for the character .</value></summary>
		class UDashComponent* dashComponent;

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

	//---Targeting---//
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Targeting", meta = ( DisplayName = "Target Mode" ) )
		/// Returns whether the current Z-targeting mode is in NONE, SIMPLE or ADVANCED.
		TEnumAsByte<ZTargetModes> targetMode = ZT_Simple;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Targeting", meta = ( DisplayName = "Target Range" ) )
		/// The max range a selected enemy can ever be in the moment of choosing.
		float targetRange = 1000;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Targeting", meta = ( DisplayName = "Maximun Target Distance" ) )
		/// The max distance a selected enemy can ever be before automatically returning to normal mode.
		float maxTargetDistance = 2000;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Targeting", meta = ( DisplayName = "Player Vision" ) )
		/// The angle of vision of the character that defines the surface where enemies can be targeted.
		FVector2D playerVisionAngle = ( 120, 80 );
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Targeting", meta = ( DisplayName = "Only Target in Vision Cone" ) )
		/// Whether or not you want the player to only target the actors that are inside the vision cone of the player.
		bool onlyTargetVisionCone = false;

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
	void MoveRight( float Val );

	/// <summary>
	/// <para>
	/// Executes movement for the character on the vertical axis.
	/// </para>
	/// </summary>
	/// <param name="Val">Input value for the vertical axis.</param>
	/// <returns>
	/// This function returns no value.
	/// </returns>
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
	void OnAttack();

	/// <summary>
	/// Execute the dash function of the dash component
	/// </summary>
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
	void EnabledSkillA();
	/// <summary>
	/// Enableds the skill b.
	/// </summary>
	void EnabledSkillB();

	/// <summary>
	/// Enableds the grapple hook.
	/// </summary>
	void ActivateGrappleHook();

	void ActivateGODMode();

	UFUNCTION(BlueprintCallable)
	void ChangeMovementState( MovementState newState);



private:

	void RotateTowardsZTarget();

	void InitComponents();
	void InitPrimitiveComponents();

	void CastRays();

//------------------------//
// AUXILIAR
//------------------------//

	/// <summary>
	/// Returns a string with the name of a movementState shared as parameter
	/// </summary>
	FString GetStateAsName( MovementState EnumValue );
};
