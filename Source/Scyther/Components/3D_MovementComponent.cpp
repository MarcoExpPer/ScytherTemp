// Fill out your copyright notice in the Description page of Project Settings.


#include "3D_MovementComponent.h"
#include <DrawDebugHelpers.h>
#include <Kismet/KismetSystemLibrary.h>
//Unreal components
#include <Components/CapsuleComponent.h>
//Game files
#include <Scyther/Player/ScytherPlayerPawn.h>


U3D_MovementComponent::U3D_MovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


//------------------------//
// UE4 OVERRIDDEN FUNCTIONS
//------------------------//
void U3D_MovementComponent::BeginPlay()
{
	Super::BeginPlay();

	jumpStartInertia = jumpStartInertia / 100;
	airControl = airControl / 100;

	player = Cast<AScytherPlayerPawn>( GetOwner() );

	jumpSpeed = jumpLength / jumpDuration;

}

void U3D_MovementComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );


#if WITH_EDITOR 
	if( movementText )
	{
		GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, FString::Printf( TEXT( "Velocity x: %.2f, y: %.2f, z: %.2f, Frame Velocity x: %.1f, y: %.1f, z: %.1f" ), velocity.X, velocity.Y, velocity.Z, addedVelocity.X, addedVelocity.Y, addedVelocity.Z ) );
		GEngine->AddOnScreenDebugMessage( -1, 0, onSlope ? FColor::Green : FColor::Red, FString::Printf( TEXT( "onSlope %d" ), onSlope ) );
		GEngine->AddOnScreenDebugMessage( -1, 0, onFlatGround ? FColor::Green : FColor::Red, FString::Printf( TEXT( "OnFlatGround %d" ), player->isGrounded ) );
	}
#endif

	updateMovement( DeltaTime );


#if WITH_EDITOR 
	if( movementText )
	{
		GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, FString::Printf( TEXT( "--- MOVEMENT COMP ---" ) ) );
	}
#endif
}

//------------------------//
// TICK MOVEMENT FUNCTIONS
//------------------------//
void U3D_MovementComponent::updateMovement( float DeltaTime )
{
	//Check if the player is on the floor, on a slope, on the air or must be knocked off a curb
	CheckFloor();

	//Update to walking/falling state depending on the previous calculations
	UpdateGroundedRelatedStates();

	//Update Horizontal Movement
	UpdateVelocityXAxis( DeltaTime );
	UpdateVelocityYAxis( DeltaTime );
	ClampHorizontalSpeed();

	//Update vertical movement
	UpdateVelocityZAxis( DeltaTime );

	//Move the character
	DoMovement( DeltaTime );

	addedVelocity.Set( 0.f, 0.f, 0.f );
}

//------------------------//
// CHECK GROUND AND SLOPES
//------------------------//
void U3D_MovementComponent::CheckFloor()
{
	CheckGrounded();
}

void U3D_MovementComponent::CheckGrounded()
{
	FVector const bottomOfTheCapsule = player->GetActorLocation() - FVector( 0, 0, player->capsule->GetUnscaledCapsuleHalfHeight() );
	float groundRayCastLength = 7;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor( GetOwner() );

	bool col = GetWorld()->LineTraceSingleByChannel( groundedHit, bottomOfTheCapsule,
		bottomOfTheCapsule + FVector::DownVector * groundRayCastLength, ECC_WorldStatic, CollisionParams );


#if WITH_EDITOR 
	if( groundedRaycast )
	{
		DrawDebugLine( GetWorld(), bottomOfTheCapsule, bottomOfTheCapsule + FVector::DownVector * groundRayCastLength,
			col ? FColor::Green : FColor::Red, false, 0, 0U, 3 );
	}
#endif


	if( col )
	{
		float angleDegrees = GetHitDegreeAngle( groundedHit );

		onFlatGround = angleDegrees == 0;
		onSlope = angleDegrees <= maxSlopeAngleDegrees && angleDegrees != 0;

		player->SetActorLocation( FVector( player->GetActorLocation().X, player->GetActorLocation().Y, groundedHit.ImpactPoint.Z + player->capsule->GetUnscaledCapsuleHalfHeight() + 1 ) );

	}
	else
	{
		onFlatGround = false;
		onSlope = false;
	}

	player->isGrounded = onSlope || onFlatGround;

	if( !player->isGrounded && player->state == MovementState::FALLING )
	{
		CheckDownStep();
	}
}

void U3D_MovementComponent::CheckDownStep()
{
	bool doDebug = false;


	FVector sphereCenter = player->GetActorLocation() - FVector( 0, 0,
		player->capsule->GetUnscaledCapsuleHalfHeight() - player->capsule->GetUnscaledCapsuleRadius() -1 );

#if WITH_EDITOR 
	if( groundedRaycast )
	{
		doDebug = true;
	}
#endif

	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add( GetOwner() );

	TArray<TEnumAsByte<EObjectTypeQuery>> objects;
	objects.Add( UEngineTypes::ConvertToObjectType( ECollisionChannel::ECC_WorldStatic ) );
	objects.Add( UEngineTypes::ConvertToObjectType( ECollisionChannel::ECC_WorldDynamic ) );
	objects.Add( UEngineTypes::ConvertToObjectType( ECollisionChannel::ECC_PhysicsBody ) );

	TArray<FHitResult> hits;
	UKismetSystemLibrary::SphereTraceMultiForObjects( GetWorld(), sphereCenter, sphereCenter + FVector::DownVector * 4,
		player->capsule->GetUnscaledCapsuleRadius() + 1, objects, true,
		actorsToIgnore, doDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, hits,
		true, FLinearColor::Green, FLinearColor::Red, 2 );

	//If in a curb, move the player out;
	for( FHitResult hit : hits )
	{
		if( hit.bBlockingHit )
		{
#if WITH_EDITOR 
			if( doDebug )
			{
				DrawDebugPoint( GetWorld(), hit.ImpactPoint, 10, FColor::Purple, false, 10, 5 );
			}
#endif
			FVector playerXY = player->GetActorLocation();
			playerXY.Z = 0;
			hit.ImpactPoint.Z = 0;

			player->SetActorLocation( player->GetActorLocation() + ( playerXY - hit.ImpactPoint ) );
		}
	}

}

void U3D_MovementComponent::UpdateGroundedRelatedStates()
{
	if( player->isGrounded &&
		( player->state == MovementState::FALLING || player->state == MovementState::INAPEX ) )
	{
		player->ChangeMovementState( MovementState::WALKING );
	}
	else if( !player->isGrounded && player->state == MovementState::WALKING )
	{
		player->ChangeMovementState( MovementState::FALLING );
	}
}


//------------------------//
// UPDATE HORIZONTAL VELOCITY
//------------------------//
void U3D_MovementComponent::UpdateVelocityXAxis( float DeltaTime )
{
	float deaceleracion;
	float control;

	switch( player->state )
	{
	case MovementState::WALKING:
		deaceleracion = horizontalDeacceleration;
		control = 1;
		break;
	case MovementState::JUMPING:
	case MovementState::INAPEX:
	case MovementState::FALLING:
	{
		deaceleracion = airHorizontalDeacceleration;
		control = airControl;
		break;
	}
	default:
	{
		return;
	}
	}


	if( addedVelocity.X == 0.0f )
	{
		if( ( velocity.X <= 0 && velocity.X >= -deaceleracion * 2 ) ||
			( velocity.X >= 0 && velocity.X <= deaceleracion * 2 ) )
		{
			velocity.X = 0;

		}
		else
		{
			if( velocity.X > 0 )
			{
				velocity.X -= deaceleracion * DeltaTime;
			}
			else
			{
				velocity.X += deaceleracion * DeltaTime;
			}
		}
	}
	else
	{
		velocity.X = velocity.X + addedVelocity.X * control * DeltaTime;
	}
}


void U3D_MovementComponent::UpdateVelocityYAxis( float DeltaTime )
{
	float deaceleracion;
	float control;

	switch( player->state )
	{
	case MovementState::WALKING:
		deaceleracion = horizontalDeacceleration;
		control = 1;
		break;
	case MovementState::JUMPING:
	case MovementState::INAPEX:
	case MovementState::FALLING:
	{
		deaceleracion = airHorizontalDeacceleration;
		control = airControl;
		break;
	}
	default:
	{
		return;
	}
	}

	if( addedVelocity.Y == 0.0f )
	{
		if( ( velocity.Y <= 0 && velocity.Y >= -deaceleracion * 2 ) ||
			( velocity.Y >= 0 && velocity.Y <= deaceleracion * 2 ) )
		{
			velocity.Y = 0;

		}
		else
		{
			if( velocity.Y > 0 )
			{
				velocity.Y -= deaceleracion * DeltaTime;
			}
			else
			{
				velocity.Y += deaceleracion * DeltaTime;
			}
		}
	}
	else
	{
		velocity.Y = velocity.Y + addedVelocity.Y * control * DeltaTime;
	}
}

void U3D_MovementComponent::ClampHorizontalSpeed()
{
	switch( player->state )
	{
	case MovementState::WALKING:
	{
		velocity = velocity.GetClampedToSize2D( -maxHorizontalSpeed, maxHorizontalSpeed );
		break;
	}
	case MovementState::JUMPING:
	case MovementState::FALLING:
	case MovementState::INAPEX:
	{
		velocity = velocity.GetClampedToSize2D( -airMaxHorizontalSpeed, airMaxHorizontalSpeed );
		break;
	}
	case MovementState::DASHING:
	{
		return;
	}
	default:
	{
		velocity = velocity.GetClampedToSize2D( -maxHorizontalSpeed, maxHorizontalSpeed );
		return;
	}
	}
}

//------------------------//
// UPDATE VERTICAL VELOCITY
//------------------------//
void U3D_MovementComponent::UpdateVelocityZAxis( float DeltaTime )
{
	switch( player->state )
	{
	case MovementState::WALKING:
	{
		velocity.Z = 0;
		currentCoyoteTime = 0;
		break;
	}
	case MovementState::JUMPING:
	{
		currentCoyoteTime += DeltaTime;
		UpdateJump( DeltaTime );
		break;
	}
	case MovementState::FALLING:
	case MovementState::INAPEX:
	{
		currentCoyoteTime += DeltaTime;
		UpdateFall( DeltaTime );
		break;
	}
	case MovementState::DASHING:
	{
		velocity.Z = 0;
		currentCoyoteTime = coyoteTime;
	}
	default:
	{
		currentCoyoteTime += DeltaTime;
		return;
	}
	}
}


void U3D_MovementComponent::UpdateJump( float DeltaTime )
{
	if( allowAceleratingJump )
	{
		velocity.Z -= jumpingGravity * DeltaTime;

		if( velocity.Z <= 0 )
		{
			player->ChangeMovementState( MovementState::FALLING );
		}
	}
	else
	{
		velocity.Z = jumpSpeed * DeltaTime;
	}

	if( headHit || endJump )
	{
		player->ChangeMovementState( MovementState::FALLING );

	}
	else if( GetOwner()->GetActorLocation().Z >= jumpStartHeight + jumpLength )
	{
		player->ChangeMovementState( MovementState::INAPEX );
	}
}

void U3D_MovementComponent::UpdateFall( float DeltaTime )
{
	if( velocity.Z <= 0 && player->state != MovementState::FALLING )
	{
		player->ChangeMovementState( MovementState::FALLING );
	}

	if( player->state == MovementState::INAPEX )
	{
		velocity.Z -= apexGravity * DeltaTime;
	}
	else if( player->state == MovementState::FALLING )
	{
		velocity.Z -= fallGravity * DeltaTime;
	}

	velocity.Z = FMath::Clamp( velocity.Z, -maxFallSpeed, maxFallSpeed * 10 );
}

//------------------------//
// APPLY VELOCITY
//------------------------//
void U3D_MovementComponent::DoMovement( float DeltaTime )
{
	FVector lastFrameLocation = GetOwner()->GetActorLocation();
	FHitResult movementHit;

	if( onSlope && ( player->state == MovementState::WALKING || player->state == MovementState::DASHING ) )
	{
		FVector projectedVector = FVector::VectorPlaneProject( velocity, groundedHit.Normal );
		projectedVector.Normalize();

		GetOwner()->SetActorLocation( lastFrameLocation + ( projectedVector * velocity.Size() * overallMovementMultiplier * DeltaTime ), true, &movementHit );

	}
	else
	{
		GetOwner()->SetActorLocation( lastFrameLocation + ( velocity * overallMovementMultiplier * DeltaTime ), true,
			&movementHit );
	}

	if( movementHit.bBlockingHit )
	{
		CheckDoMovementHit( movementHit, DeltaTime );
	}

}

void U3D_MovementComponent::CheckDoMovementHit( FHitResult& hit, float DeltaTime )
{
	int const maxHitIterations = 5;
	int hitIterations = 0;

	while( hit.bBlockingHit && hitIterations <= 5 )
	{
		hitIterations++;



#if WITH_EDITOR 
		if( stepUpRaycast )
		{
			DrawDebugPoint( GetWorld(), hit.ImpactPoint, 10, FColor::Red, false, 10, 5 );
		}	
#endif

		float angleDegrees = GetHitDegreeAngle( hit );

#if WITH_EDITOR 
		if( movementText )
		{
			GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, FString::Printf( TEXT( "Angle %.2f" ), angleDegrees ) );
		}
#endif


		//Si el movimiento choca contra una cuesta que es ascendible, recoloca al player encima de la cuesta
		if( angleDegrees <= maxSlopeAngleDegrees )
		{
			FVector projectedVector = FVector::VectorPlaneProject( velocity, hit.Normal );
			projectedVector.Normalize();

			GetOwner()->SetActorLocation( GetOwner()->GetActorLocation() + ( projectedVector * velocity.Size() * overallMovementMultiplier * DeltaTime ), true, &hit );

		}
		/* Si el angulo es de 90 grados, esta chocando contra una pared o contra un bordillo para intentar subirlo*/
		else
		{
			if( !CheckUpStep() )
			{
				if( angleDegrees > 88 && angleDegrees < 92 )
				{
					GetOwner()->SetActorLocation( GetOwner()->GetActorLocation() + FVector( 0, 0, velocity.Z * overallMovementMultiplier * DeltaTime ), true, &hit );
					if( player->state == MovementState::DASHING )
					{
						player->state = MovementState::FALLING;
					}
				}
				/*Si es mayor de 90 grados, quiere decir que en la parte donde una rampa/pared y
				el suelo bajo el jugador forman un angulo agudo, en cuyo caso el jugador debe parar de ascender ya que ha golpeado
				contra la rampa/techo/pared */

				else
				{
					if( player->state != MovementState::FALLING && !player->isGrounded )
					{
						velocity.Z = 0;
						player->ChangeMovementState( MovementState::FALLING );
					}
					GetOwner()->SetActorLocation( GetOwner()->GetActorLocation() + FVector( 0, 0, velocity.Z * overallMovementMultiplier * DeltaTime ), true, &hit );
				}
			}
			break;
		}

	}

}


bool U3D_MovementComponent::CheckUpStep()
{
	if( player->state != MovementState::WALKING )
	{
		return false;
	}

	FVector velocityXY = velocity.IsZero() ? addedVelocity : velocity;
	velocityXY.Z = 0;
	velocityXY.Normalize();

	FVector verticalStart = player->GetActorLocation() + velocityXY * ( player->capsule->GetUnscaledCapsuleRadius() + 10 );
	FVector verticalEnd = verticalStart + FVector::DownVector * ( player->capsule->GetUnscaledCapsuleHalfHeight() );

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor( GetOwner() );

	FHitResult hit;
	GetWorld()->LineTraceSingleByChannel( hit, verticalStart, verticalEnd, ECC_WorldStatic, CollisionParams );

#if WITH_EDITOR 
	if( stepUpRaycast )
	{
		DrawDebugPoint( GetWorld(), verticalStart, 10, FColor::Green, false, 10, 5 );
		DrawDebugLine( GetWorld(), verticalStart, verticalEnd, FColor::Blue, false, 10, 0U, 10 );
		DrawDebugPoint( GetWorld(), hit.ImpactPoint, 10, FColor::Red, false, 10, 5 );
	}
#endif

	FVector const bottomOfTheCapsule = player->GetActorLocation() - FVector( 0, 0, player->capsule->GetUnscaledCapsuleHalfHeight() );
	float zImpact = hit.ImpactPoint.Z;
	if( hit.bBlockingHit
		&& ( zImpact - bottomOfTheCapsule.Z ) <= maxStepUpHeight
		&& ( zImpact - bottomOfTheCapsule.Z ) > 0 )
	{
		FVector floorOffset = FVector::UpVector * 0.5;
		FVector horizontalStart = bottomOfTheCapsule + floorOffset;
		FVector horizontalEnd = horizontalStart + velocityXY * player->capsule->GetUnscaledCapsuleHalfHeight();

		GetWorld()->LineTraceSingleByChannel( hit, horizontalStart, horizontalEnd, ECC_WorldStatic, CollisionParams );

#if WITH_EDITOR 
		if( stepUpRaycast )
		{
			DrawDebugPoint( GetWorld(), horizontalStart, 10, FColor::Emerald, false, 10, 5 );
			DrawDebugLine( GetWorld(), horizontalStart, horizontalEnd, FColor::Cyan, false, 10, 0U, 10 );
			DrawDebugPoint( GetWorld(), hit.ImpactPoint, 10, FColor::Purple, false, 10, 5 );
		}
#endif
		if( hit.bBlockingHit )
		{
			player->SetActorLocation( FVector( hit.ImpactPoint.X, hit.ImpactPoint.Y,
				zImpact + player->capsule->GetUnscaledCapsuleHalfHeight() + 1 ) );

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}



//------------------------//
// PLAYER INPUT FUNCTIONS
//------------------------//

void U3D_MovementComponent::AddHorizontalMovement( FVector direction, float scale )
{
	switch( player->state )
	{
	case MovementState::WALKING:
	{
		addedVelocity = ( scale * direction * horizontalAcceleration ) + addedVelocity;
	}
	case MovementState::JUMPING:
	case MovementState::INAPEX:
	case MovementState::FALLING:
	{
		addedVelocity = ( scale * direction * airHorizontalAcceleration ) + addedVelocity;
		break;
	}
	default:
	{
		return;
	}
	}
}

void U3D_MovementComponent::JumpStart()
{
	if( player->state == MovementState::WALKING )
	{
		currentCoyoteTime = coyoteTime;

		initiateJump();
	}
	else if( currentCoyoteTime < coyoteTime )
	{
		initiateJump();
	}
}

void U3D_MovementComponent::JumpStop()
{
	if( allowEarlyEndJump && player->state == MovementState::JUMPING )
	{
		if( fastEarlyJumpFall )
		{
			player->ChangeMovementState( MovementState::FALLING );
			velocity.Z = -maxFallSpeed;
		}
		else
		{
			player->ChangeMovementState( MovementState::INAPEX );
		}

		jumpEndWhilePreparing = true;
	}
}

void U3D_MovementComponent::initiateJump()
{
	currentCoyoteTime = coyoteTime;
	jumpEndWhilePreparing = false;

	jumpStartHeight = GetOwner()->GetActorLocation().Z;

	velocity *= jumpStartInertia;

	player->ChangeMovementState( MovementState::JUMPING );

	if( allowAceleratingJump )
	{
		velocity.Z = jumpEndWhilePreparing ? jumpAcceleration / 2 : jumpAcceleration;
	}
}

void U3D_MovementComponent::jumpPreparationFinished()
{
	initiateJump();
}

//------------------------//
// AUXILIAR
//------------------------//
void U3D_MovementComponent::rotateTowardsVelocity()
{
	if( velocity.X != 0 || velocity.Y != 0 )
	{
		FVector xy = FVector( velocity.X, velocity.Y, 0 );
		GetOwner()->SetActorRotation( xy.Rotation() );

	}
}

float U3D_MovementComponent::GetHitDegreeAngle( FHitResult& hit )
{
	hit.Normal.Normalize();

	return FMath::Abs(
		FMath::Acos( FVector::DotProduct( hit.Normal, FVector::DownVector ) ) *
		180 / PI - 180
	);
}


