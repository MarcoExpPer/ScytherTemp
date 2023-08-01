// Fill out your copyright notice in the Description page of Project Settings.

#include "ScytherPlayerPawn.h"
#include "UObject/Object.h"
#include "UObject/ScriptDelegates.h"
#include "Math/UnrealMathUtility.h"
#include <DrawDebugHelpers.h>
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
//Components
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

//Primitive Components
#include "../Components/AttackComponent.h"
#include "../Components/GrappleHookComponent.h"
#include "../Components/TargetingComponent.h"
#include "../Components/SkillComponent.h"
#include "../Components/3D_MovementComponent.h"
#include "../Components/DashComponent.h"
#include "../Components/EntityShadow.h"
#include "../Components/ScythePosController.h"
#include <Scyther/Components/HealthComponent.h>
#include <Scyther/Components/BloodComponent.h>
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include <Scyther/Components/TransverseComponent.h>
#include "GameFramework/CharacterMovementComponent.h"

//------------------------//
//      CONSTRUCTOR
//------------------------//

// Sets default values
AScytherPlayerPawn::AScytherPlayerPawn()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitComponents();
	InitPrimitiveComponents();
}

void AScytherPlayerPawn::ApplyKnockback( FVector knockbackStrength, float knockbackDuration )
{
	state = MovementState::KNOCKBACK;
	movementComp->velocity = knockbackStrength;

}

void AScytherPlayerPawn::InitComponents()
{
	capsule = CreateDefaultSubobject<UCapsuleComponent>( TEXT( "Capsule" ) );
	SetRootComponent( capsule );
	capsule->ComponentTags.Add( FName( "PlayerBody" ) );

	mesh = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "Mesh" ) );
	mesh->SetupAttachment( RootComponent );

	SphereSkillCollision = CreateDefaultSubobject<USphereComponent>( TEXT( "Skill" ) );
	SphereSkillCollision->SetupAttachment( RootComponent );

	// SCYTHE
	scytheMesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "Scythe" ) );
	//scytheMesh->SetupAttachment( mesh, "RightHand" );

	scytheAttackCol = CreateDefaultSubobject<UBoxComponent>( TEXT( "ScytheEdgeCollision" ) );
	scytheAttackCol->SetupAttachment( scytheMesh, "DamageCollisionSocket" );
	scytheAttackCol->SetGenerateOverlapEvents( false );

	verticalAttackCol = CreateDefaultSubobject<UCapsuleComponent>( TEXT( "VerticalAttackCollision" ) );
	verticalAttackCol->SetupAttachment( RootComponent );
	verticalAttackCol->SetGenerateOverlapEvents( false );

	// CAMERA
	springArm = CreateDefaultSubobject<USpringArmComponent>( TEXT( "Spring Arm" ) );
	springArm->SetupAttachment( RootComponent );
	springArm->SetUsingAbsoluteRotation( true ); // Rotation of the character should not affect rotation of spring
	springArm->bDoCollisionTest = false;
	springArm->TargetArmLength = 500.f;
	springArm->SocketOffset = FVector( 0.f, 0.f, 75.f );
	springArm->SetRelativeRotation( FRotator( 0.f, 180.f, 0.f ) );
	FVector fv = springArm->GetRelativeTransform().GetLocation();
	UE_LOG( LogTemp, Warning, TEXT( "%f %f %f" ), fv.X, fv.Y, fv.Z );

	// Create a camera and attach to spring
	playerCamera = CreateDefaultSubobject<UCameraComponent>( TEXT( "Camera" ) );
	playerCamera->SetupAttachment( springArm );
	playerCamera->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	//Niagara Systems
	//Dash
	dashNiagaraSystem = CreateDefaultSubobject<UNiagaraComponent>( TEXT( "DashNiagaraSystem" ) );
	dashNiagaraSystem->SetupAttachment( mesh );
	dashNiagaraSystem->SetCustomDepthStencilValue( 2 );
	dashAvailableNiagaraSystem = CreateDefaultSubobject<UNiagaraComponent>( TEXT( "DashAvailableNiagaraSystem" ) );
	dashAvailableNiagaraSystem->SetupAttachment( mesh );
	dashAvailableNiagaraSystem->SetCustomDepthStencilValue( 2 );
	//Other
	niagaraSystem = CreateDefaultSubobject<UNiagaraComponent>( TEXT( "NiagaraSystem" ) );
	niagaraSystem->SetupAttachment( RootComponent );
	niagaraSystem->SetCustomDepthStencilValue( 2 );

	//CircleRingShadow
	circleRingShadow = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "CircleRingShadow" ) );
	circleRingShadow->SetupAttachment( RootComponent );

	//EntityShadow
	entityShadow = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "EntityShadow" ) );
	entityShadow->SetupAttachment( RootComponent );	
	

}

void AScytherPlayerPawn::InitPrimitiveComponents()
{
	healthComponent = CreateDefaultSubobject<UHealthComponent>( TEXT( "Health Component" ) );
	AddOwnedComponent( healthComponent );
	healthComponent->saveLastHp = true;

	bloodComponent = CreateDefaultSubobject<UBloodComponent>( TEXT( "Blood Component" ) );
	AddOwnedComponent( bloodComponent );

	AIPercetionComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>( TEXT( "AI Perception Component" ) );
	AddOwnedComponent( AIPercetionComponent );

	targetComponent = CreateDefaultSubobject<UTargetingComponent>( TEXT( "Targeting Component" ) );
	AddOwnedComponent( targetComponent );

	skillComponent = CreateDefaultSubobject<USkillComponent>( TEXT( "Skill Component" ) );
	AddOwnedComponent( skillComponent );

	movementComp = CreateDefaultSubobject<U3D_MovementComponent>( TEXT( "Movement Component" ) );
	AddOwnedComponent( movementComp );

	attackComponent = CreateDefaultSubobject<UAttackComponent>( TEXT( "Attack Component" ) );
	AddOwnedComponent( attackComponent );

	grappleHookComponent = CreateDefaultSubobject<UGrappleHookComponent>( TEXT( "Grapple Hook Component" ) );
	AddOwnedComponent( grappleHookComponent );

	dashComponent = CreateDefaultSubobject<UDashComponent>( TEXT( "Dash Component" ) );
	AddOwnedComponent( dashComponent );

	transverseComponent = CreateDefaultSubobject<UTransverseComponent>( TEXT( "Traverse Component" ) );
	AddOwnedComponent( transverseComponent );

	entityShadowComponent = CreateDefaultSubobject<UEntityShadow>( TEXT( "Entity Shadow Component" ) );
	AddOwnedComponent( entityShadowComponent );

	scythePosComponent = CreateDefaultSubobject<UScythePosController>( TEXT( "Scythe Position Controller" ) );
	AddOwnedComponent( scythePosComponent );
}

//------------------------//
//   BEGIN PLAY | TICK
//------------------------//

// Called when the game starts or when spawned
void AScytherPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AScytherPlayerPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

#if WITH_EDITOR 
	if( stateText )
	{
		GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, GetStateAsName( state ) );
	}

	if( playerLocationText )
	{
		GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, FString::Printf( TEXT( "PlayerLocation x: %.1f, y: %.1f, z: %.1f" ), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z ) );
	}
#endif
}


//------------------------//
//       BIND INPUT
//------------------------//

// Called to bind functionality to input
void AScytherPlayerPawn::SetupPlayerInputComponent( UInputComponent* PlayerInputComponent )
{
	Super::SetupPlayerInputComponent( PlayerInputComponent );

}


//------------------------//
//       MOVIMIENTO
//------------------------//

void AScytherPlayerPawn::MoveRight( float Val )
{
	FVector movementDirection = FVector( 1.f, 0.f, 0.f );

	AActor* targetEnemy = targetComponent->GetTarget();
	if( targetEnemy )
	{
		FVector enemyDirection = targetEnemy->GetActorLocation() - GetActorLocation();
		float x = 0;
		float axisChange = Val < 0 ? 1 : -1;
		switch( targetComponent->targetMode )
		{
		case ZT_None:
			movementComp->AddHorizontalMovement( movementDirection, Val );

			break;

		case ZT_Simple:
			if( !enemyTargeted )
			{
				enemyTargeted = true;
			}
			RotateTowardsZTarget();
			movementComp->AddHorizontalMovement( movementDirection, Val );
			break;

		case ZT_Advanced:
			if( !enemyTargeted )
			{
				enemyTargeted = true;
			}
			RotateTowardsZTarget();
			//Vector perpendicular al la dirección entre el player y el enemigo
			enemyDirection.Y *= -1;
			x = enemyDirection.X;
			enemyDirection.X = enemyDirection.Y;
			enemyDirection.Y = x;
			//Eliminamos el componente Z
			enemyDirection.Z = 0;
			//Rotación de 20 grados o -20 grados al vector
			//Es el ángulo que permite que el jugador se transporte entre puntos de la circunferencia de desplazamientoç
			//Sin acercarse ni alejarse
			enemyDirection = enemyDirection.RotateAngleAxis( axisChange * 20, FVector( 0, 0, 1 ) );
			movementComp->AddHorizontalMovement( enemyDirection, Val );
			break;

		default:
			break;
		}
	}
	else
	{
		if( enemyTargeted )
		{
			enemyTargeted = false;
		}
		movementComp->AddHorizontalMovement( movementDirection, Val );
	}
}

/// <summary>
/// Función que ejecuta el movimiento en el eje vertical del jugador
/// </summary>
/// <param name="Val"> Input del jugador. Valor recibido del axis. </param>
void AScytherPlayerPawn::MoveForward( float Val )
{
	FVector movementDirection = FVector( 0.f, -1.f, 0.f );

	AActor* targetEnemy = targetComponent->GetTarget();
	if( targetEnemy )
	{
		FVector enemyDirection = targetEnemy->GetActorLocation() - GetActorLocation();
		switch( targetComponent->targetMode )
		{
		case ZT_None:
			movementComp->AddHorizontalMovement( movementDirection, Val );
			break;

		case ZT_Simple:
			if( !enemyTargeted )
			{
				enemyTargeted = true;
			}
			RotateTowardsZTarget();
			movementComp->AddHorizontalMovement( movementDirection, Val );
			break;

		case ZT_Advanced:
			if( !enemyTargeted )
			{
				enemyTargeted = true;
			}
			RotateTowardsZTarget();
			//Eliminamos el componente Z
			enemyDirection.Z = 0;
			//Normalizamos
			enemyDirection.Normalize( 1 );
			movementComp->AddHorizontalMovement( enemyDirection, Val );
			break;

		default:
			break;
		}
	}
	else
	{
		if( enemyTargeted )
		{
			enemyTargeted = false;
		}
		movementComp->AddHorizontalMovement( movementDirection, Val );
	}
}


//------------------------//
//         SALTO
//------------------------//

int a = 0;
void AScytherPlayerPawn::OnJumpPressed()
{
	movementComp->JumpStart();
}

void AScytherPlayerPawn::OnJumpReleased()
{
	movementComp->JumpStop();
}

//------------------------//
//         DASH
//------------------------//

void AScytherPlayerPawn::OnDash()
{
	dashComponent->DashPressed();
}

void AScytherPlayerPawn::OnBloodRecover()
{
	float healthMissing = healthComponent->maxHp - healthComponent->currentHp;
	healthComponent->changeHealth( bloodComponent->useBlood( healthMissing ), DamageModes::HEALING );
}

//------------------------//
//        ZTARGET
//------------------------//

void AScytherPlayerPawn::TargetPressed()
{
	if( targetComponent->GetTarget() )
	{
		targetComponent->SetTarget( nullptr );
	}
	else
	{
		targetComponent->FindTarget();
	}
}

void AScytherPlayerPawn::TargetSwitchRight( float value )
{
	if( targetComponent->GetTarget() )
	{
		targetComponent->SwitchTargetRight( value );
		//UE_LOG( LogTemp, Warning, TEXT( "Derecha: %f" ), value );
	}
}

void AScytherPlayerPawn::TargetSwitchUp( float value )
{
	if( targetComponent->GetTarget() )
	{
		targetComponent->SwitchTargetUp( value );
		//UE_LOG( LogTemp, Warning, TEXT( "Arriba: %f" ), value );
	}
}

//------------------------//
//         SKILLS
//------------------------//

void AScytherPlayerPawn::EnabledSkillA()
{
	skillComponent->DarkSkill();
}

void AScytherPlayerPawn::EnabledSkillB()
{
	skillComponent->LightSkill();
}

void AScytherPlayerPawn::ChangeMovementState( MovementState newState, bool fixPlayerToGround )
{
	if( newState == MovementState::ATTACKING )
	{
		movementComp->velocity = FVector::ZeroVector;

		state = newState;
		return;
	}

	if( movementComp->CheckJumpBufferAndDoJump( newState ) )
	{
		return;
	}

	//Always set the player at the ground
	if( newState == MovementState::WALKING || fixPlayerToGround )
	{
		if( movementComp->castRayToGround( GetActorLocation() - FVector( 0, 0, capsule->GetUnscaledCapsuleHalfHeight() - 1 ), 20 ))
		{
			state = newState;

			if( movementComp->setPlayerAtGround() )
			{
				state = MovementState::WALKING;
			}
			return;
		}
	}

	state = newState;

	if( newState == MovementState::KNOCKBACK )
	{
		attackComponent->OnAttackEnded();
	}
}

//------------------------//
//         ATTACK
//------------------------//

void AScytherPlayerPawn::OnAttack()
{
	attackComponent->OnAttack();
}

//------------------------//
//     GRAPPLE HOOK
//------------------------//

void AScytherPlayerPawn::ActivateGrappleHook()
{
	grappleHookComponent->ActivateGrappleHook();
}

//------------------------//
//     Traverse Mode
//------------------------//
void AScytherPlayerPawn::MoveToTheNextPos( FTransform x )
{
	transverseComponent->MoveToTheNextPos( x );
}

void AScytherPlayerPawn::MoveToThePreviousPos( FTransform x )
{
	transverseComponent->MoveToThePreviousPos( x );
}

//------------------------//
//     GOD Mode
//------------------------//



void AScytherPlayerPawn::ActivateGODMode()
{
	if( isGodMode )
	{

#if WITH_EDITOR 
		GEngine->AddOnScreenDebugMessage( -1, 3, FColor::Black, FString::Printf( TEXT( "GOD MODE DESACTIVADO" ) ), true, FVector2D( 5, 5 ) );
#endif
		isGodMode = false;


		//Change all components to normal mode
		//No recibo daño
		healthComponent->becomeMortal();
		bloodComponent->becomeMortal();
		//No Gasto mana
		skillComponent->isGodMode = false;

		//Infinitedamage
		attackComponent->isGodMode = false;
		//endGodMode();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage( -1, 3, FColor::Black, FString::Printf( TEXT( "GOD MODE ACTIVADO" ) ), true, FVector2D( 5, 5 ) );

		isGodMode = true;

		//Change all components to god mode user
		//No recibo daño
		healthComponent->becomeGod();
		bloodComponent->becomeGod();

		//No Gasto mana
		skillComponent->isGodMode = true;

		//Infinitedamage
		attackComponent->isGodMode = true;
		//startGodMode();
	}
}

//------------------------//
//  FUNCIONALIDAD EXTRA
//------------------------//

void AScytherPlayerPawn::RotateTowardsZTarget()
{
	//Actual rotation
	FRotator actualRotation = GetActorRotation();

	AWormPawn* wormPawn = Cast<AWormPawn>( targetComponent->GetTarget() );
	FVector enemyTargetRotation;

	if( wormPawn )
	{
		enemyTargetRotation = FVector( wormPawn->wormCapsule->GetComponentLocation().X, wormPawn->wormCapsule->GetComponentLocation().Y, GetActorLocation().Z );
	}
	else
	{
		enemyTargetRotation = targetComponent->GetTarget()->GetActorLocation();
	}

	//Target rotation
	FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation( GetActorLocation(), enemyTargetRotation );
	targetRotation.Pitch = actualRotation.Pitch;
	targetRotation.Roll = actualRotation.Roll;

	FRotator rotation = FMath::RInterpTo(
		actualRotation,
		targetRotation,
		GetWorld()->GetDeltaSeconds(),
		0
	);

	SetActorRelativeRotation( rotation );
}

//------------------------//
// AUXILIAR
//------------------------//

FString AScytherPlayerPawn::GetStateAsName( MovementState EnumValue )
{
	const UEnum* EnumPtr = FindObject<UEnum>( ANY_PACKAGE, TEXT( "MovementState" ), true );
	if( !EnumPtr ) return FString( "Invalid" );
	return EnumPtr->GetNameByValue( (int64) EnumValue ).ToString();
}