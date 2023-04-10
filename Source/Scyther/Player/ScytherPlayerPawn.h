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

	//------------------------//
	//   RAY CASTS
	//------------------------//

	//------------------------//
	//  PARAMETERS
	//------------------------//


	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
	MovementState state = MovementState::FALLING;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
	bool isGrounded;

	//Dash
	bool firstDash = false;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Dash", meta = ( DisplayName = "DashForce" ) )
		/// The higher, the more distance the character will be able to cover when performing a dash.
		float dashForce = 500;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Dash", meta = ( DisplayName = "NextDashAvailabilty" ) )
		/// Minimum number of seconds that the character has to wait from the end of a dash
		/// to be able to start a new one.
		float waitForTheNextDash = 3;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Dash", meta = ( DisplayName = "DashOnGround" ) )
		/// If true, allows the player to dash while making contact with the ground
		bool dashOnGround = true;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Dash" )
		/// If true, allows the player to dash while making contact with the ground
		bool allowDashOnAir = true;

	bool isGodMode = false;

	private:
		MovementState animationPreDash;
	public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Dash", meta = ( DisplayName = "PlayRateOfDash" ) )
		/// The higher it is, the more accurate the dash will be rendered in game, and the more resources it will need.
		float dashRate = 1;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Dash", meta = ( DisplayName = "TraceDashEffect" ) )
		/// The VFX the character will leave behind while dashing.
		UParticleSystem* traceDashEffect;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Dash", meta = ( DisplayName = "SetCurveFloat1" ) )
		/// The linear curve that represents the progression of the movement that the character follows when dashing.
		UCurveFloat* curveFloat;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design|Dash", meta = ( DisplayName = "SetCurveFloat2" ) )
		/// The linear curve that represents the progression of the movement that the camera follows after dashing.
		UCurveFloat* curveFloat2;

	//-- EXPERIENCE --/ /--deprecated--/
public:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Design|Experience" )
		float currentExperience = 0;

	//---Dash---//
private:
	/// Refers to the relative position the character will get when colliding in a dash.
	const float capsuleRadius = -55;
	/// Returns the remaining time of the dash that is currently being performed.
	float currentDashTime;
	/// Return true when time since the last dash is lower than the cooldown time.
	bool hasDashed;
	/// Refers to the location the character was in at the exact moment of pressing the dash button.
	FVector currentLocation;
	/// Refers to the location the character will be in at the exact moment of finishing a dash movement.
	FVector destination;
	/// Refers to the timeline that will be executed to interpolate the character position during a dash movement.
	FTimeline curveTimeLine;
	/// Refers to the timeline that will be executed to interpolate the camera position after a dash movement.
	FTimeline curveCameraTimeLine;
	/// Delegate that stores the callback function after the character finishes a dash.
	FOnTimelineEventStatic onDashFinished;

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

	FOnTimelineFloat timelIneProgress;
	FOnTimelineFloat cameraTimelIneProgress;

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
	/// <para>
	/// Checks whether the player can dash or not and whether it collides against a wall or not,
	/// then calls <see cref="DashExecution"/>.
	/// </para>
	/// <para>
	/// Not finished.
	/// </para>
	/// </summary>
	/// <param name="No params"/>
	/// <returns>
	/// This function returns no value.
	/// </returns>
	void OnDash();

	/// <summary>
	/// <para>
	/// Executes the dash animation and logic for the player.
	/// </para>
	/// <para>
	/// Not finished.
	/// </para>
	/// </summary>
	/// <param name="dashDirection">Vector containing the direction and speed of the movement.</param>
	/// <returns>
	/// This function returns no value.
	/// </returns>
	void DashExecution( FVector dashDirection );

	UFUNCTION()
		void TimelineProgress( float val );
	UFUNCTION()
		void CameraTimelineProgress( float val );

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

	UFUNCTION()
		void DashFinishedCallback();


	void InitComponents();
	void InitPrimitiveComponents();

	void CastRays();
};
