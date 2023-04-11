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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCYTHER_API U3D_MovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | DEBUG " )
		bool movementText = true;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | DEBUG " )
		bool stepUpRaycast = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | DEBUG " )
		bool groundedRaycast = true;

private:
	class AScytherPlayerPawn* player;
	//------------------------//
	//   Horizontal Movement
	//------------------------//
	FVector addedVelocity;

public:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
	FVector velocity;
	//horizontalAcceleration es la aceleracion que se aplica cuando el jugador se mueve en el eje horizontal mientras camina/corre por el suelo.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Horizontal Movement ", meta=(Units = "cm"))
	float horizontalAcceleration = 277;
	//horizontalDeacceleration es la deaceleracion que se aplica cuando el jugador camina/corre por el suelo y "suelta" el joystick.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Horizontal Movement ", meta =(Units ="cm") )
	float horizontalDeacceleration = 500;
	//maxHorizontalSpeed velocidad maxima que puede alcanzar el jugador al caminar/correr.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Horizontal Movement ", meta = ( Units = "cm" ) )
	float maxHorizontalSpeed = 500;

	//------------------------//
	//   AIR Horizontal Movement
	//------------------------//
	//airHorizontalAcceleration hace lo mismo que horizontalAcceleration pero cuando el jugador esta en el aire
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | AIR Horizontal Movement ", meta = ( Units = "cm" ) )
		float airHorizontalAcceleration = 277;
	//airHorizontalDeacceleration hace lo mismo que horizontalDeacceleration pero cuando el jugador esta en el aire
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | AIR Horizontal Movement ", meta = ( Units = "cm" ) )
		float airHorizontalDeacceleration = 500;
	//airMaxHorizontalSpeed hace lo mismo que airMaxHorizontalSpeed pero cuando el jugador esta en el aire
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | AIR Horizontal Movement ", meta = ( Units = "cm" ) )
		float airMaxHorizontalSpeed = 500;

	//jumpStartInertia es el porcentaje de movimiento horizontal que se traslada al movimiento aereo cuando el jugador salta. Un 0 implica que el jugador siempre empieza saltando en el eje Z sin ninguna velocidad en el eje X / Y
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | AIR Horizontal Movement ", meta = ( Units = "%" ) )
		float jumpStartInertia = 100;

	//airControl es el porcentaje de airHorizontalAcceleration que se aplica mientras el jugador esta en el aire.Realmente poner aqui un 50 % es lo mismo que reducir airHorizontalAcceleration a la mitad. Si este valor se pone a 0, el jugador no tendra ningun tipo de control durante el salto.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | AIR Horizontal Movement ", meta = ( Units = "%" ) )
		float airControl = 50;

	//------------------------//
	//   Jump
	//------------------------//
	//allowEarlyEndJump permite que al soltar el boton de saltar, el salto entre en el modo de apex para empezar a caer y terminarse antes de tiempo
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Jump " )
		bool allowEarlyEndJump = true;
	//fastEarlyJumpFall permite que cuando se termina el salto antes de tiempo, se pase al modo caida a la maxima velocidad de caida.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Jump " )
		bool fastEarlyJumpFall = false;
	//allowAceleratingJump si esta a falso, el ascenso del salto se hace a velocidad constante.Si esta a true, el ascenso se hace con una aceleracion base y una gravedad.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | AceleratingJump " )
		bool allowAceleratingJump = true;

	//jumpDuration solo se tiene en cuenta si allowAceleratingJump esta a falso. Se utiliza para calcular la velocidad constante de subida
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Jump ", meta = ( Units = "s" ) )
	float jumpDuration = 1;
	//jumpLength si allowAceleratingJump esta a falso, se utiliza para calcular la velocidad constante.Si esta a true, cuandoe el jugador sobrepasa esta altura, cambia el personaje al estado de apex
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Jump ", meta = ( Units = "cm" ) )
	float jumpLength = 250;

private:
	bool jumpEndWhilePreparing = false;
public:

	//jumpAcceleration si allowAceleratingJump esta a true, esta variable indica la velocidad aplicada al inicio del salto
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | AceleratingJump " )
	float jumpAcceleration = 1500;
	//jumpingGravity si allowAceleratingJump esta a true, indica cuanta gravedad se aplica al jugador.
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | AceleratingJump " )
	float jumpingGravity = 38;

	//------------------------//
	//   FALL
	//------------------------//
	//apexGravity gravedad aplicada mientras el jugador esta en el estado de apex
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | AceleratingJump " )
	float apexGravity = 38;

	//fallGravity gravedad aplicada mientras el jugador esta en estado de caida
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Fall ", meta = ( Units = "cm" ) )
	float fallGravity = 56;
	//maxFallSpeed velocidad maxima de caida
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Fall ", meta = (Units = "cm") )
	float maxFallSpeed = 1000;

	//------------------------//
	//   UTILITIES
	//------------------------//
	//coyoteTime tiempo maximo para aprovechar el coyote time
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Utilities ", meta = ( Units = "s" ) )
	float coyoteTime = 0.3;
private:
	float currentCoyoteTime = 0;
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Utilities ", meta = (Units = "cm" ) )
		float maxStepUpHeight = 60.f;
public:
	//overallMovementMultiplier multiplicador a toda la velocidad que recibe el jugador en un instante concreto. Si esta a 0, el jugador no puede moverse
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Utilities " )
	float overallMovementMultiplier = 1;

private:
	float jumpSpeed;
	float jumpStartHeight;

	//Variables to check if a jump has finished
	bool endJump = false;

	//------------------------//
	//   GROUNDED AND SLOPES
	//------------------------//
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Slope Movement ")
	float maxSlopeAngleDegrees = 21.f;

	FHitResult groundedHit;
	FHitResult headHit;

	FHitResult slopeHit;
	bool onSlope;
	bool onFlatGround;

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
	/// Cast a ray to check if the player is in the ground. If the ray hits, calculates if its a slope or a flat floor.
	/// If the player is not in the ground, check if it can stepDown
	/// </summary>
	void CheckGrounded();

	/// <summary>
	/// Check if the player is close to a curb. In such case, push the player down to evade weird capsule-bottom collision bugs
	/// </summary>
	void CheckDownStep();

	/// <summary>
	/// Update falling and walking state depending on the grounded condition
	/// </summary>
	void UpdateGroundedRelatedStates();

	/// <summary>
	/// Check if the ray above player head is hitting something
	/// </summary>
	void CheckHead();
	//------------------------//
	// UPDATE HORIZONTAL VELOCITY
	//------------------------//
	void UpdateVelocityXAxis( float DeltaTime );
	void UpdateVelocityYAxis( float DeltaTime );

	/// <summary>
	/// Clamp the horizontal speed to not go beyond the max horizontal speed
	/// </summary>
	void ClampHorizontalSpeed();

	//------------------------//
	// UPDATE VERTICAL VELOCITY
	//------------------------//

	/// <summary>
	/// Updates vertical velocity/gravity and coyote timer.
	/// </summary>
	void UpdateVelocityZAxis( float DeltaTime );

	/// <summary>
	/// Updates vertical velocity/gravity when the player is jumping/rising in the jump. It also swaps from jumping to 
	/// falling when the vertical speed reachs 0 and from jumping to apex when the jump surpass the maxheightJump
	/// </summary>
	void UpdateJump( float DeltaTime );

	/// <summary>
	/// Update vertical velocity/gravity when the player is falling or in apex state. It also swaps from apex to falling state
	/// </summary>
	void UpdateFall( float DeltaTime );

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
	void CheckDoMovementHit( FHitResult& hit, float DeltaTime );

	/// <summary>
	/// Check if the player can stepUp the curb that is on front of him during a checkDoMovementHit. It uses two different raycasts
	/// to calculate the height of the curb and where the player should be "teleported" to.
	/// </summary>
	bool CheckUpStep();

	//------------------------//
	// PLAYER INPUT FUNCTIONS
	//------------------------//
public:
	/// <summary>
	/// Add horizontal movement in a direction and in a scale. During the tick, this accumulated horizontal speed will be 
	/// used
	/// </summary>
	UFUNCTION( BlueprintCallable )
	void AddHorizontalMovement( FVector direction, float scale );

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
	/// Function to rotate the player towards it's current accumulated velocity
	/// </summary>
	UFUNCTION( BlueprintCallable )
	void rotateTowardsVelocity();

private:
	/// <summary>
	/// Returns the Degree Angle between the hit normal and the down vector
	/// </summary>
	float GetHitDegreeAngle( FHitResult& hit );
};
