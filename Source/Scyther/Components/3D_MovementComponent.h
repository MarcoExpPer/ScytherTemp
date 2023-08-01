/*
Autor: Marco Expósito Pérez.

Last Update: 25/03/2023

File Overview:
Este componente controla el movimiento horizontal y el salto del personaje del jugador.

El movimiento horizontal se calcula tick a tick, acumulando la aceleracion (o deaceleracion) horizontal en addedVelocity. Velocity es la velocidad actual, a la cual cada tick se le añade
addedVelocity y se clampea segun la velocidad maxima vigente.

El salto tiene 3 fases:

- Ascenso/saltando, se entra a esta fase cuando el jugador pulsa el boton de salto y se termina cuando su velocidad de ascenso llega a 0 o cuando alcanza la altitud del salto.
- Apex se entra cuando el ascenso ha terminado. Es un periodo en el que el salto esta empezando a terminar y hace la tipica curvatura del salto. Termina cuando la velocidad de caida alcanza la gravedad de caida
- Caida se entra cuando el jugador se cae, cuando el salto termina antes de tiempo de forma rapida y cuando el apex ha terminado
*/
#pragma once

#include "CoreMinimal.h"
//Unreal components
#include "Components/ActorComponent.h"
#include "3D_MovementComponent.generated.h"

//Forward declaration
enum class MovementState: uint8;

UENUM()
enum DetectedFloor
{
	NO_FLOOR,
	FLAT_FLOOR,
	WALKLABLE_SLOPE,
	BIG_SLOPE
};

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class SCYTHER_API U3D_MovementComponent: public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|DEBUG " )
		bool movementText = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|DEBUG " )
		bool stepUpRaycast = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|DEBUG " )
		bool stepDownRaycast = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|DEBUG " )
		bool checkIfLocationisSfaeDebug = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|DEBUG " )
		bool groundedRaycast = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|DEBUG " )
		bool velocityRaycast = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|DEBUG " )
		bool wallSlidecast = false;
private:
	class AScytherPlayerPawn* player;
	//------------------------//
	//   Horizontal Movement
	//------------------------//

	bool isFastTurning = false;
	FVector addedVelocity;

public:
	FVector playerFreeInput;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
		FVector velocity = FVector::ZeroVector;
	//horizontalAcceleration es la aceleracion que se aplica cuando el jugador se mueve en el eje horizontal mientras camina/corre por el suelo.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Horizontal Movement", meta = ( Units = "cm" ) )
		float horizontalAcceleration = 277;
	//horizontalDeacceleration es la deaceleracion que se aplica cuando el jugador camina/corre por el suelo y "suelta" el joystick.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Horizontal Movement", meta = ( Units = "cm" ) )
		float horizontalDeacceleration = 500;
	//knockback deaceleration es la deaceleracion que se aplica cuando el jugador recibe un knockback y aun no ha recuperado el control.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Horizontal Movement", meta = ( Units = "cm" ) )
		float knockBackDeacceleration = 1950;
	//maxHorizontalSpeed velocidad maxima que puede alcanzar el jugador al caminar/correr.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Horizontal Movement", meta = ( Units = "cm" ) )
		float maxHorizontalSpeed = 500;
	//Treshold para autoajustar la direccion de movimiento del personaje a los ejes principales
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Horizontal Movement", meta = ( Units = "cm" ) )
		float horizontalStopThreshold = 50;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Horizontal Movement" )
		float extraTurningSpeedMultiplier = 7500;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Horizontal Movement" )
		float extraTurningSpeedMultiplierAngleLimit = 5;

	//Angulo maximo para detectar si el jugador esta haciendo un cambio brusco de direccion.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Horizontal Movement|fastTurn" )
		float maxFastTurnAngle = 135;
	//Deaceleracion utilizada cuando el jugador esta intentado hacer un giro rapido
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Horizontal Movement|fastTurn" )
		float fastTurnDeaceleration = 8000;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Horizontal Movement|fastTurn" )
		bool applyExtraTurnOnFastTurn = true;

	//Velocidad de giro del personaje en grados por segundo.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Horizontal Movement" )
		float rotatingSpeed = 10.f;

	//------------------------//
	//   AIR Horizontal Movement
	//------------------------//
	//airHorizontalAcceleration hace lo mismo que horizontalAcceleration pero cuando el jugador esta en el aire
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|AIR Horizontal Movement", meta = ( Units = "cm" ) )
		float airHorizontalAcceleration = 277;
	//airHorizontalDeacceleration hace lo mismo que horizontalDeacceleration pero cuando el jugador esta en el aire
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|AIR Horizontal Movement", meta = ( Units = "cm" ) )
		float airHorizontalDeacceleration = 500;
	//airMaxHorizontalSpeed hace lo mismo que airMaxHorizontalSpeed pero cuando el jugador esta en el aire
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|AIR Horizontal Movement", meta = ( Units = "cm" ) )
		float airMaxHorizontalSpeed = 500;

	//jumpStartInertia es el porcentaje de movimiento horizontal que se traslada al movimiento aereo cuando el jugador salta. Un 0 implica que el jugador siempre empieza saltando en el eje Z sin ninguna velocidad en el eje X / Y
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|AIR Horizontal Movement", meta = ( Units = "%" ) )
		float jumpStartInertia = 100;

	//airControl es el porcentaje de airHorizontalAcceleration que se aplica mientras el jugador esta en el aire.Realmente poner aqui un 50 % es lo mismo que reducir airHorizontalAcceleration a la mitad. Si este valor se pone a 0, el jugador no tendra ningun tipo de control durante el salto.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|AIR Horizontal Movement", meta = ( Units = "%" ) )
		float airControl = 50;

	//------------------------//
	//   Jump
	//------------------------//
	//allowEarlyEndJump permite que al soltar el boton de saltar, el salto entre en el modo de apex para empezar a caer y terminarse antes de tiempo
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Jump" )
		bool allowEarlyEndJump = true;
	//fastEarlyJumpFall permite que cuando se termina el salto antes de tiempo, se pase al modo caida a la maxima velocidad de caida.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Jump" )
		bool fastEarlyJumpFall = false;
	//allowAceleratingJump si esta a falso, el ascenso del salto se hace a velocidad constante.Si esta a true, el ascenso se hace con una aceleracion base y una gravedad.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Jump" )
		bool allowAceleratingJump = true;

	//jumpDuration solo se tiene en cuenta si allowAceleratingJump esta a falso. Se utiliza para calcular la velocidad constante de subida
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Jump", meta = ( Units = "s" ) )
		float jumpDuration = 1;
	//jumpLength si allowAceleratingJump esta a falso, se utiliza para calcular la velocidad constante.Si esta a true, cuandoe el jugador sobrepasa esta altura, cambia el personaje al estado de apex
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Jump", meta = ( Units = "cm" ) )
		float jumpLength = 250;

private:
	bool jumpEndWhilePreparing = false;

public:

	//jumpAcceleration si allowAceleratingJump esta a true, esta variable indica la velocidad aplicada al inicio del salto
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|AceleratingJump" )
		float jumpAcceleration = 1500;
	//jumpingGravity si allowAceleratingJump esta a true, indica cuanta gravedad se aplica al jugador.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|AceleratingJump" )
		float jumpingGravity = 38;

	//------------------------//
	//   FALL
	//------------------------//
	//apexGravity gravedad aplicada mientras el jugador esta en el estado de apex
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|AceleratingJump" )
		float apexGravity = 38;

	//fallGravity gravedad aplicada mientras el jugador esta en estado de caida
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Fall", meta = ( Units = "cm" ) )
		float fallGravity = 56;
	//maxFallSpeed velocidad maxima de caida
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Fall", meta = ( Units = "cm" ) )
		float maxFallSpeed = 1000;

	//------------------------//
	//   VERTICAL DROP
	//------------------------//
	//Gravedad aplicada al jugador en caso de estar haciendo el ataque en caida
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Vertical Drop", meta = ( Units = "cm" ) )
		float dropGravity = 112;
	//Velocidad maxima de caida en el ataque en caida
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Vertical Drop", meta = ( Units = "cm" ) )
		float maxDropSpeed = 3000;

	//------------------------//
	//   UTILITIES
	//------------------------//
	//coyoteTime tiempo maximo para aprovechar el coyote time
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Utilities", meta = ( Units = "s" ) )
		float coyoteTime = 0.3;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Utilities" )
		float wallKnockback = 100.f;

private:
	float currentCoyoteTime = 0;

	FTimerHandle jumpBufferTimerHandle;
public:
	bool isJumpSaved = false;
	bool isJumpBtnCurrentlyPressed = false;

	//Si el jugador pulsa el boton de saltar mientras esta en el aire, cuantos segundos se guardan esta "pulsacion" para que cuando toque el suelo se vuelva a ejecutar el salto
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Utilities", meta = ( Units = "s" ) )
		float jumpBufferDuration = 1.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Utilities", meta = ( Units = "cm" ) )
		float maxStepUpHeight = 60.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Utilities", meta = ( Units = "cm" ) )
		float maxStepDownHeight = 70.f;

public:
	//overallMovementMultiplier multiplicador a toda la velocidad que recibe el jugador en un instante concreto. Si esta a 0, el jugador no puede moverse
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Utilities" )
		float overallMovementMultiplier = 1;

	bool isMovementActive = true;

private:
	float jumpSpeed = 0;
	float jumpStartHeight = 0;

	//Variables to check if a jump has finished
	bool endJump = false;

	//------------------------//
	//   GROUNDED AND SLOPES
	//------------------------//
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component|Slope Movement" )
		float maxSlopeAngleDegrees = 21.f;

	FHitResult groundedHit;
	FHitResult headHit;
	FHitResult slopeHit;

	DetectedFloor detectedFloor = DetectedFloor::NO_FLOOR;

	FVector velocityxyNormalized;

public:
	U3D_MovementComponent();

	//------------------------//
	// UE4 OVERRIDDEN FUNCTIONS
	//------------------------//
	void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

protected:
	void BeginPlay() override;
	//------------------------//
	// TICK MOVEMENT FUNCTIONS
	//------------------------//
private:
	/// <summary>
	/// Update all player movement based on the player input and the accumulated velocity
	/// </summary>
	void updateMovement( float DeltaTime );

	//------------------------//
	// CHECK GROUND AND SLOPES
	//------------------------//

	/// <summary>
	/// Check the floor below the player to know if its grounded.
	/// </summary>
	void CheckRays();

	/// <summary>
	/// Cast a ray to check if the player is in the ground. If the ray hits, calculates if its a flat floor, a walkable slope or a big slope updating detectedFloor and player->grounded 
	/// variable with the results.
	/// In any case, this also updates groundedHit variable
	/// </summary>
	void CheckGrounded();

	/// <summary>
	/// Check if the player is close to the edge of a surface. If the player is at the edge of a surface the player will be pushedBack out of the edge. From this new location,
	/// a raycast will check if the player can "downStep" to a floor below the new location and will return true if it efectively moves to this new floor
	/// </summary>
	bool CheckDownStep();

	/// <summary>
	/// Cast a ray to check if the player head is hitting something. Updates headHit variable.
	/// </summary>
	void CheckHead();
	//------------------------//
	// UPDATE HORIZONTAL VELOCITY
	//------------------------//
	void UpdateHorizontalVelocityAxis( float DeltaTime );
	void UpdateVelocityXAxis( float DeltaTime, float deaceleracion, float control );
	void UpdateVelocityYAxis( float DeltaTime, float deaceleracion, float control );

	/// <summary>
	/// Clamp the horizontal speed to not go beyond the max horizontal speed
	/// </summary>
	void ClampHorizontalSpeed();

	//------------------------//
	// UPDATE VERTICAL VELOCITY
	//------------------------//

	/// <summary>
	/// Updates vertical velocity and coyote timer depending on the movement state.
	/// </summary>
	void UpdateVelocityZAxis( float DeltaTime );

	/// <summary>
	/// Updates vertical speed in the first state of a jump. If the jump ends or reachs the maxJumpHeight, the player will start to fall or will start the apex of the jump
	/// </summary>
	void UpdateJump( float DeltaTime );

	/// <summary>
	/// Updates vertical speed once the player is already falling or in the apex of the jump. Updates the apex into falling when necesary. Clamps the vertical speed
	/// </summary>
	void UpdateFall( float DeltaTime );

	/// <summary>
	/// Update vertical velocity/gravity when the player is droping from a vertical attack
	/// </summary>
	void UpdateVerticalDrop( float DeltaTime );

	//------------------------//
	// APPLY VELOCITY
	//------------------------//

	/// <summary>
	/// Takes the final velocity, and execute the movement to actualy move the player around.
	/// </summary>
	void DoMovement( float DeltaTime );

	/// <summary>
	/// Check if the final movement has hit something. In such case, check if its hitting against a slope to move the player on
	/// top of the slope, if its hitting against a wall, to remove horizontal movement from the final movement or if its hitting
	/// against a walkable step, to move the player on top of it.
	/// </summary>
	void CheckDoMovementHit( FHitResult& hit, float DeltaTime, FVector VOffset, int iterationsLeft );

	/// <summary>
	/// Check if the player can stepUp the curb that is on front of him during a checkDoMovementHit. It uses two different raycasts
	/// to calculate the height of the curb and where the player should be "teleported" to.
	/// </summary>
	bool CheckUpStep( FVector movementVector, FVector& newplayerLocation );

	//------------------------//
	// PLAYER INPUT FUNCTIONS
	//------------------------//
public:
	/// <summary>
	/// Add horizontal movement in a direction and in a scale. During the tick, this accumulated horizontal speed will be 
	/// used
	/// </summary>
	UFUNCTION( BlueprintCallable )
		void AddHorizontalMovement( FVector direction, float scale, bool isFromPlayerInput = true );

	bool checkIfPlayerCanBeFixedToGround();

private:
	/// <summary>
	/// Check if the player is doing a fastTurn, if the player input regards velocity is bigger than a maxFastTurnAngle. Updates isFastTurning variable with the result.
	/// It also checks if the player normal turning velocity must be increased.
	/// </summary>
	void UpdateTurningVelocity( FVector playerInput, float DeltaTime );

	/// <summary>
	/// Increase the normal turning velocity of the player
	/// </summary>
	void increaseTurningSpeed( float turnAngle, FVector playerInput, float DeltaTime );

public:
	/// <summary>
	/// Check if the player can start a jump (because it's grounded or coyote time is available) and initiate the jump
	/// </summary>
	UFUNCTION( BlueprintCallable )
		void JumpStart();

	/// <summary>
	/// Change player state to apex or to falling depending on parameters.
	/// </summary>
	UFUNCTION( BlueprintCallable )
		void JumpStop();

	/// <summary>
	/// Starts the player jump changing its state, updating the coyote time and the Z velocity
	/// </summary>
	UFUNCTION( BlueprintCallable )
		void initiateJump();

	/// <summary>
	/// Flag to use from an animation blueprint to actualy start the jump. Currently unused because the jump happens
	/// at the same time as the player press the button
	/// </summary>
	UFUNCTION( BlueprintCallable )
		void jumpPreparationFinished();

	//------------------------//
	// AUXILIAR
	//------------------------//

	/// <summary>
	/// Rotates the player towards a vector using rotationSpeed parameter
	/// </summary>
	UFUNCTION( BlueprintCallable )
		void rotateTowardsVector( float DeltaTime, FVector targetVector );

	/// <summary>
	/// AnimNotify: Function to stop completly the movement
	/// </summary>
	UFUNCTION( BlueprintCallable )
		void OnMovementCompletlyStops();

	/// <summary>
	/// Returns if the jump was able to be executed executed
	/// </summary>
	bool CheckJumpBufferAndDoJump( MovementState newState );

	bool castRayToGround( FVector initialLocation, float length = 15, bool longerDebug = false );
	bool setPlayerAtGround();

	bool isAJumpingState( MovementState stateToCheck );

	UFUNCTION( BlueprintCallable )
		void DisableMovementForXSeconds( float seconds, FVector locationToFixPlayer );



private:

	void EnableMovement();

	/// <summary>
	/// Returns the Degree Angle between the hit normal and the down vector
	/// </summary>
	float GetHitDegreeAngle( FHitResult& hit );

	void timerHandleFinished();

	bool CheckIfLocationIsSafeForPlayer( FVector location, TArray<UPrimitiveComponent*>& OverlappedComps, bool moveTo, FHitResult* hit = nullptr );

	FString GetStateAsName( DetectedFloor EnumValue );

	void ChangeDetectedFloor( DetectedFloor newDetectedFloor );

	bool HeadFreeToJump();
};
