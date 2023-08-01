// Fill out your copyright notice in the Description page of Project Settings.


#include "3D_MovementComponent.h"
#include <DrawDebugHelpers.h>
#include <Kismet/KismetSystemLibrary.h>
//Unreal components
#include <Components/CapsuleComponent.h>
#include "Kismet/GameplayStatics.h"
//Game files
#include <Scyther/Player/ScytherPlayerPawn.h>
#include <Scyther/Components/TargetingComponent.h>
#include <Scyther/Components/DashComponent.h>

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
		FString velocityString = FString::Printf( TEXT( "Velocity x: %.2f, y: %.2f, z: %.2f, Frame Velocity x: %.1f, y: %.1f, z: %.1f" ), velocity.X, velocity.Y, velocity.Z,
addedVelocity.X, addedVelocity.Y, addedVelocity.Z );
		GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, velocityString );


		FString stateName = GetStateAsName( detectedFloor );
		GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, stateName );
	}
#endif

	if( isMovementActive )
	{
		//initialize some vectors that will be used all around the tick function
		velocityxyNormalized = FVector( velocity.X, velocity.Y, 0 );
		velocityxyNormalized.Normalize();

		//Cast different rays to know if the player is on the floor or hitting its head against something
		CheckRays();

		updateMovement( DeltaTime );

		//Finish a knockback
		if( player->state == MovementState::KNOCKBACK )
		{
			if( velocity.IsNearlyZero() )
			{
				velocity = FVector::ZeroVector;
				player->ChangeMovementState( MovementState::FALLING );
			}
		}

		//Rotate the player rotation (not the movement rotation) to look to the speed vector.
		if( !player->targetComponent->GetTarget() )
		{
			rotateTowardsVector( DeltaTime, velocity );
		}

		if( !isMovementActive )
		{
			velocity = FVector::ZeroVector;
		}
	}
	else
	{
		velocity = FVector::ZeroVector;
	}

	addedVelocity.Set( 0.f, 0.f, 0.f );
	playerFreeInput.Set( 0.f, 0.f, 0.f );

}

//------------------------//
// TICK MOVEMENT FUNCTIONS
//------------------------//

void U3D_MovementComponent::updateMovement( float DeltaTime )
{
	//Update Horizontal Movement
	UpdateTurningVelocity( addedVelocity, DeltaTime );

	UpdateHorizontalVelocityAxis( DeltaTime );
	ClampHorizontalSpeed();

	//Update vertical movement
	UpdateVelocityZAxis( DeltaTime );

	//Move the character
	DoMovement( DeltaTime );
}

//------------------------//
// CHECK GROUND, SLOPES AND HEAD
//------------------------//
void U3D_MovementComponent::CheckRays()
{
	CheckGrounded();
	CheckHead();
}

void U3D_MovementComponent::CheckGrounded()
{
	DetectedFloor newDetected;

	if( castRayToGround( player->GetActorLocation() - FVector( 0, 0, player->capsule->GetUnscaledCapsuleHalfHeight() - 3) ) )
	{
		float floorAngle = GetHitDegreeAngle( groundedHit );

		if( floorAngle == 0 )
		{
			newDetected = DetectedFloor::FLAT_FLOOR;
		}
		else if( floorAngle <= maxSlopeAngleDegrees )
		{
			newDetected = DetectedFloor::WALKLABLE_SLOPE;
		}
		else
		{
			newDetected = DetectedFloor::BIG_SLOPE;
		}
	}
	else
	{
		newDetected = DetectedFloor::NO_FLOOR;
	}

	player->isGrounded = newDetected == DetectedFloor::NO_FLOOR ? false : true;

	ChangeDetectedFloor( newDetected );
}

void U3D_MovementComponent::ChangeDetectedFloor( DetectedFloor newDetectedFloor )
{
	if( newDetectedFloor != detectedFloor )
	{
		detectedFloor = newDetectedFloor;

		if( !isAJumpingState( player->state ) )
		{
			setPlayerAtGround();
		}
	}
}
bool U3D_MovementComponent::CheckDownStep()
{
	FVector sphereCenter = player->GetActorLocation() - FVector( 0, 0,
		player->capsule->GetUnscaledCapsuleHalfHeight() - player->capsule->GetUnscaledCapsuleRadius() - 2 );

	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add( GetOwner() );

	TArray<TEnumAsByte<EObjectTypeQuery>> objects;
	objects.Add( UEngineTypes::ConvertToObjectType( ECollisionChannel::ECC_WorldStatic ) );
	//objects.Add( UEngineTypes::ConvertToObjectType( ECollisionChannel::ECC_WorldDynamic ) );


	TArray<FHitResult> hits;
	UKismetSystemLibrary::SphereTraceMultiForObjects( GetWorld(), sphereCenter, sphereCenter + FVector::DownVector * 8, player->capsule->GetUnscaledCapsuleRadius(), objects, false,
		actorsToIgnore, EDrawDebugTrace::None, hits, true, FLinearColor::Green, FLinearColor::Red, 2 );

	//If there is a hit, it means that we are in the edge of something
	for( FHitResult hit : hits )
	{
		if( hit.bBlockingHit )
		{
			FVector playerxy = player->GetActorLocation();
			playerxy.Z = 0;
			hit.ImpactPoint.Z = 0;

			FVector knockback = ( playerxy - hit.ImpactPoint );
			knockback.Normalize();
			knockback *= 50;

			FVector endLocation = player->GetActorLocation() + knockback;

			bool longDebug = false;
#if WITH_EDITOR 
			if( stepDownRaycast )
			{
				longDebug = true;
				DrawDebugLine( GetWorld(), player->GetActorLocation(), endLocation, FColor::Purple, false, 15, 0U, 5 );
				DrawDebugPoint( GetWorld(), endLocation, 20, FColor::Blue, false, 15 );
			}
#endif


			if( castRayToGround( endLocation - FVector( 0, 0, player->capsule->GetUnscaledCapsuleHalfHeight() - 1 ), maxStepDownHeight, longDebug ) )
			{
				if( setPlayerAtGround() )
				{
					return true;
				}
			}

			GetOwner()->SetActorLocation( endLocation );
			return false;
		}
	}

	return false;
}

void U3D_MovementComponent::CheckHead()
{
	FVector const topOfTheCapsule = player->GetActorLocation() + FVector( 0, 0, player->capsule->GetUnscaledCapsuleHalfHeight() );
	float headRayCastLength = 20;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor( GetOwner() );

	//Check if something WORLD_STATIC is on top of the player capsule
	bool col = GetWorld()->LineTraceSingleByObjectType( headHit, topOfTheCapsule, topOfTheCapsule + FVector::UpVector * headRayCastLength,
		FCollisionObjectQueryParams( ECC_WorldStatic ), CollisionParams );

#if WITH_EDITOR 
	if( groundedRaycast )
	{
		DrawDebugLine( GetWorld(), topOfTheCapsule, topOfTheCapsule + FVector::UpVector * headRayCastLength,
			col ? FColor::Green : FColor::Red, false, 0, 0U, 5 );
	}
#endif
}
//------------------------//
// UPDATE HORIZONTAL VELOCITY
//------------------------//


void U3D_MovementComponent::UpdateHorizontalVelocityAxis( float DeltaTime )
{
	float deaceleracion;
	float control = 1;

	switch( player->state )
	{
	case MovementState::WALKING:
	{
		deaceleracion = isFastTurning ? fastTurnDeaceleration : horizontalDeacceleration;;
		break;
	}
	case MovementState::JUMPING:
	case MovementState::INAPEX:
	case MovementState::FALLING:
	{
		deaceleracion = airHorizontalDeacceleration;
		control = airControl;
		break;
	}
	case MovementState::ATTACKING:
	case MovementState::PREPARING_DROP:
	case MovementState::VERTICAL_DROP:
	{
		deaceleracion = horizontalDeacceleration;
		break;
	}
	case MovementState::KNOCKBACK:
	{
		deaceleracion = knockBackDeacceleration;
		break;
	}
	default:
	{
		return;
	}
	}


	UpdateVelocityXAxis( DeltaTime, deaceleracion, control );
	UpdateVelocityYAxis( DeltaTime, deaceleracion, control );
}



void U3D_MovementComponent::UpdateVelocityXAxis( float DeltaTime, float deaceleracion, float control )
{
	if( addedVelocity.X == 0.0f || isFastTurning )
	{
		if( ( velocity.X < 0 && velocity.X >= -horizontalStopThreshold ) ||
			( velocity.X > 0 && velocity.X <= horizontalStopThreshold ) )
		{
			velocity.X = 0;

		}
		else
		{
			if( velocity.X > 0 )
			{
				velocity.X += ( -deaceleracion ) * DeltaTime;

				if( velocity.X < 0 ) velocity.X = 0;
			}
			else
			{

				velocity.X += (deaceleracion) *DeltaTime;

				if( velocity.X > 0 ) velocity.X = 0;
			}
		}
	}
	else
	{
		velocity.X = velocity.X + ( addedVelocity.X * control * DeltaTime );
	}
}


void U3D_MovementComponent::UpdateVelocityYAxis( float DeltaTime, float deaceleracion, float control )
{
	if( addedVelocity.Y == 0.0f || isFastTurning )
	{
		if( ( velocity.Y < 0 && velocity.Y >= -horizontalStopThreshold ) ||
			( velocity.Y > 0 && velocity.Y <= horizontalStopThreshold ) )
		{
			velocity.Y = 0;
		}
		else
		{
			if( velocity.Y > 0 )
			{
				velocity.Y += ( -deaceleracion ) * DeltaTime;

				if( velocity.Y < 0 ) velocity.Y = 0;
			}
			else
			{
				velocity.Y += (deaceleracion) *DeltaTime;

				if( velocity.Y > 0 ) velocity.Y = 0;
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
		if( isFastTurning )
		{
			velocity = velocity.GetClampedToSize2D( -maxHorizontalSpeed * 10, maxHorizontalSpeed * 10 );
		}
		else
		{
			velocity = velocity.GetClampedToSize2D( -maxHorizontalSpeed, maxHorizontalSpeed );
		}
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
	case MovementState::ATTACKING:
	case MovementState::PREPARING_DROP:
	case MovementState::VERTICAL_DROP:
	{
		return;
	}
	case MovementState::NONE:
	{
		velocity = velocity * 0.9;
		return;
	}
	default:
	{
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
	case MovementState::ATTACKING:
	case MovementState::PREPARING_DROP:
	{
		velocity.Z = 0;
		currentCoyoteTime = coyoteTime;
		break;
	}
	case MovementState::VERTICAL_DROP:
	{
		currentCoyoteTime += DeltaTime;
		UpdateVerticalDrop( DeltaTime );
		break;
	}
	case MovementState::KNOCKBACK:
	{
		velocity.Z -= apexGravity * DeltaTime;
		if( velocity.Z <= 0 )
		{
			velocity.Z = 0;
		}
		break;
	}
	case MovementState::DASHING:
	default:
	{
		currentCoyoteTime += DeltaTime;
		return;
	}
	}
}


void U3D_MovementComponent::UpdateJump( float DeltaTime )
{
	if( headHit.bBlockingHit )
	{
		velocity.Z = 0;
		player->ChangeMovementState( MovementState::FALLING );
		return;
	}

	if( !allowAceleratingJump )
	{
		velocity.Z = jumpSpeed * DeltaTime;
	}
	else
	{
		velocity.Z -= jumpingGravity * DeltaTime;
	}

	if( velocity.Z <= 0 )
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
	if( player->state == MovementState::INAPEX )
	{
		velocity.Z -= apexGravity * DeltaTime;
	}
	else
	{
		velocity.Z -= fallGravity * DeltaTime;
	}

	if( velocity.Z <= 0 && player->state == MovementState::INAPEX )
	{
		player->ChangeMovementState( MovementState::FALLING );
	}

	velocity.Z = FMath::Clamp( velocity.Z, -maxFallSpeed, maxFallSpeed * 10 );
}

void U3D_MovementComponent::UpdateVerticalDrop( float DeltaTime )
{
	velocity.Z -= dropGravity * DeltaTime;

	velocity.Z = FMath::Clamp( velocity.Z, -maxDropSpeed, maxDropSpeed * 10 );
}

//------------------------//
// APPLY VELOCITY
//------------------------//
void U3D_MovementComponent::DoMovement( float DeltaTime )
{
	FHitResult movementHit;
	FVector actorLocation = GetOwner()->GetActorLocation();

	switch( detectedFloor )
	{
	case DetectedFloor::NO_FLOOR:
	{
		//If the groundRay doesnt find floor, we may be in the edge of a surface. We check if this is the case to move the player out of the surface
		if( !isAJumpingState( player->state ) || player->state == MovementState::FALLING )
		{
			if( CheckDownStep() )
			{
				return;
			}
			else
			{
				if( player->state != MovementState::DASHING  &&
					player->state != MovementState::PREPARING_DROP &&
					player->state != MovementState::VERTICAL_DROP &&
					player->state != MovementState::KNOCKBACK )
				{
					player->ChangeMovementState( MovementState::FALLING );
				}
				
			}
		}

		GetOwner()->SetActorLocation( actorLocation + ( velocity * overallMovementMultiplier * DeltaTime ), true, &movementHit );

		break;
	}
	case DetectedFloor::FLAT_FLOOR:
	{
		if( player->state != MovementState::JUMPING &&
			isAJumpingState( player->state ) )
		{
			player->ChangeMovementState( MovementState::WALKING );
		}

		if( player->state != MovementState::FALLING )
		{
			GetOwner()->SetActorLocation( actorLocation + ( velocity * overallMovementMultiplier * DeltaTime ), true, &movementHit );
		}
		break;
	}

	case DetectedFloor::WALKLABLE_SLOPE:
	{
		if( player->state != MovementState::JUMPING &&
			isAJumpingState( player->state ) )
		{
			player->ChangeMovementState( MovementState::WALKING );
		}

		if( player->state == MovementState::JUMPING )
		{
			GetOwner()->SetActorLocation( actorLocation + ( velocity * overallMovementMultiplier * DeltaTime ), true, &movementHit );
			break;
		}

		FVector velocityxy = FVector( velocity.X, velocity.Y, 0 );
		FVector slopeDirection = FVector::VectorPlaneProject( velocityxy, groundedHit.Normal );
		slopeDirection.Normalize();

		GetOwner()->SetActorLocation( actorLocation + ( slopeDirection * velocity.Size() * overallMovementMultiplier * DeltaTime ), true, &movementHit );

		break;
	}
	case DetectedFloor::BIG_SLOPE:
	{
		if( !isAJumpingState( player->state ) || velocity.Z <= 0 )
		{
			if( CheckDownStep() )
			{
				return;
			}
			else
			{
				player->ChangeMovementState( MovementState::FALLING );
			}
		}

		FVector slopeDirection = FVector::VectorPlaneProject( velocity, groundedHit.Normal );

		GetOwner()->SetActorLocation( actorLocation + ( slopeDirection * overallMovementMultiplier * DeltaTime ), true, &movementHit );
		break;
	}
	}

	if( movementHit.bBlockingHit )
	{
		FVector newPlayerLocation = GetOwner()->GetActorLocation();
		if( CheckUpStep( velocity, newPlayerLocation ) )
		{
			if( castRayToGround( newPlayerLocation + FVector(0, 0, player->capsule->GetUnscaledCapsuleHalfHeight()), 100) )
			{
				setPlayerAtGround();
			}
		}
		else
		{

			FVector VDifferentece = GetOwner()->GetActorLocation() - actorLocation;
			
			CheckDoMovementHit( movementHit, DeltaTime, VDifferentece, 10 );
		}
	}
}

void U3D_MovementComponent::CheckDoMovementHit( FHitResult& hit, float DeltaTime, FVector VOffset, int iterationsLeft )
{
	FHitResult newHit;

	if( iterationsLeft <= 0 )
		return;

	
#if WITH_EDITOR 
	if( wallSlidecast )
	{
		DrawDebugPoint( GetWorld(), hit.ImpactPoint, 10, FColor::Emerald, false, 0, 100 );

		DrawDebugLine( GetWorld(), hit.ImpactPoint, hit.ImpactPoint + (hit.ImpactNormal * 10), FColor::Emerald, false, 0, 100, 2 );
	}
#endif

	float angleDegrees = GetHitDegreeAngle( hit );

	//If we are hitting the floor
	if( angleDegrees > -2 && angleDegrees < 2 )
	{
		/*if( castRayToGround(GetOwner()->GetActorLocation() - FVector(0, 0, player->capsule->GetUnscaledCapsuleHalfHeight() - 1)) )
		{
			setPlayerAtGround();
		}*/
	}
	FVector location = GetOwner()->GetActorLocation();
	FVector planePerpendicular = hit.ImpactNormal;

	planePerpendicular = FVector( -planePerpendicular.Y, planePerpendicular.X, planePerpendicular.Z );

	FVector velocityProjectedOnHitPlane;
	velocityProjectedOnHitPlane = velocity.ProjectOnTo( planePerpendicular );

#if WITH_EDITOR 
	if( wallSlidecast )
	{
		DrawDebugLine( GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + velocityProjectedOnHitPlane, FColor::Emerald, false, 0, 100, 2 );
	}
#endif
	

	velocityProjectedOnHitPlane = FVector::VectorPlaneProject( FVector(velocity.X - VOffset.X/DeltaTime, velocity.Y-VOffset.Y/DeltaTime, velocity.Z ), hit.Normal);

#if WITH_EDITOR 
	if( wallSlidecast )
	{
		DrawDebugLine( GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + velocityProjectedOnHitPlane, FColor::Orange, false, 0, 100, 2 );
	}
#endif
	

	FVector knockback = FVector( hit.ImpactNormal.X * wallKnockback, hit.ImpactNormal.Y * wallKnockback, 0 );
	velocityProjectedOnHitPlane += knockback;
	velocityProjectedOnHitPlane.Z = velocity.Z - VOffset.Z/DeltaTime;

#if WITH_EDITOR 
	if( wallSlidecast )
	{
		DrawDebugLine( GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + velocityProjectedOnHitPlane, FColor::Red, false, 0, 100, 2 );
	}
#endif

	FVector velocityOnSlope = FVector::ZeroVector;

	//If we hit a wall
	if( angleDegrees > 85 && angleDegrees < 95)
	{
		if( player->state == MovementState::DASHING )
		{
			player->dashComponent->OnDashFinish();
		}
	}
	//If hit an inverted slope (We are hitting our head in the angled wall
	else if( angleDegrees >= 92 )
	{
		if( isAJumpingState( player->state ) )
		{
			if( velocity.Z > 0 )
			{
				velocityProjectedOnHitPlane.Z = 0;
				velocity.Z = 0;
			}
		}

	}else if (angleDegrees > maxSlopeAngleDegrees )
	{

	//if( !isAJumpingState( player->state ) )
		//velocityProjectedOnHitPlane = FVector::VectorPlaneProject( velocityProjectedOnHitPlane, groundedHit.Normal ); 
	
	}
	//If we hit a slope we can actualy walk on
	else
	{
		return;
	}
	
	if( (detectedFloor == DetectedFloor::WALKLABLE_SLOPE || detectedFloor == DetectedFloor::BIG_SLOPE) && player->state != MovementState::JUMPING )
	{
		velocityOnSlope = FVector::VectorPlaneProject( velocityProjectedOnHitPlane, groundedHit.Normal );

		GetOwner()->SetActorLocation( GetOwner()->GetActorLocation() + ( velocityOnSlope * overallMovementMultiplier * DeltaTime ), true, &newHit );
	}
	else
	{
		GetOwner()->SetActorLocation( ( GetOwner()->GetActorLocation() + velocityProjectedOnHitPlane * overallMovementMultiplier * DeltaTime ), true, &newHit );
	}

	if( newHit.bBlockingHit )
	{
 		FVector newDirection = FVector( 0, 0, velocityProjectedOnHitPlane.Z );
		GetOwner()->SetActorLocation( GetOwner()->GetActorLocation() + ( newDirection * overallMovementMultiplier * DeltaTime ), true, &newHit );
	}
}

bool U3D_MovementComponent::CheckUpStep( FVector movementVector, FVector &newplayerLocation )
{
	if( player->state != MovementState::WALKING && player->state != MovementState::DASHING )
	{
		return false;
	}

	FVector velocityXY = movementVector;
	velocityXY.Z = 0;
	velocityXY.Normalize();

	FVector verticalStart = newplayerLocation + velocityXY * ( player->capsule->GetUnscaledCapsuleRadius() + 10 );
	FVector verticalEnd = verticalStart + FVector::DownVector * ( player->capsule->GetUnscaledCapsuleHalfHeight() );

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor( GetOwner() );

	FHitResult hit;
	GetWorld()->LineTraceSingleByObjectType( hit, verticalStart, verticalEnd,
		FCollisionObjectQueryParams( ECC_WorldStatic ), CollisionParams );



#if WITH_EDITOR 
	if( stepUpRaycast )
	{
		DrawDebugPoint( GetWorld(), verticalStart, 10, FColor::Red, false, 20, 10 );
		DrawDebugLine( GetWorld(), verticalStart, verticalEnd, FColor::Blue, false, 20, 0U, 5 );
		DrawDebugPoint( GetWorld(), hit.ImpactPoint, 10, FColor::Red, false, 20, 10 );
	}
#endif

	FVector const bottomOfTheCapsule = newplayerLocation - FVector( 0, 0, player->capsule->GetUnscaledCapsuleHalfHeight() );
	float zImpact = hit.ImpactPoint.Z;

	float heightDifference = zImpact - bottomOfTheCapsule.Z;
	//If the height is stepAbble
	if( hit.bBlockingHit
		&& heightDifference <= maxStepUpHeight
		&& heightDifference > 0 )
	{
		//IF the new "floor" is a slope that we can climb
		float angleDegrees = GetHitDegreeAngle( hit );
		if( angleDegrees > maxSlopeAngleDegrees )
		{
			return false;
		}

		FVector floorOffset = FVector::UpVector * 1;
		FVector horizontalStart = bottomOfTheCapsule + floorOffset;
		FVector horizontalEnd = horizontalStart + velocityXY * player->capsule->GetUnscaledCapsuleHalfHeight();

		FHitResult hHit;
		GetWorld()->LineTraceSingleByObjectType( hHit, horizontalStart, horizontalEnd,
			FCollisionObjectQueryParams( ECC_WorldStatic ), CollisionParams );

#if WITH_EDITOR 
		if( stepUpRaycast )
		{
			DrawDebugPoint( GetWorld(), horizontalStart, 20, FColor::Emerald, false, 20, 10 );
			DrawDebugLine( GetWorld(), horizontalStart, horizontalEnd, FColor::Cyan, false, 20, 0U, 1 );
			DrawDebugPoint( GetWorld(), hHit.ImpactPoint, 10, FColor::Emerald, true, 100, 5);
		}
#endif
		//IF the horizontal hit has hit
		if( hHit.bBlockingHit )
		{
			//If there is enought space to fit player capsule inside the new step.
			FVector newPossibleLocation = FVector( hHit.ImpactPoint.X, hHit.ImpactPoint.Y, zImpact );
			newPossibleLocation += velocityXY * 2;
			newplayerLocation = newPossibleLocation;

			TArray<UPrimitiveComponent*> OverlappedComps;

			bool isSafe = CheckIfLocationIsSafeForPlayer( newPossibleLocation, OverlappedComps, true, &hit );
			return isSafe;
		}
	}

	return false;
}



//------------------------//
// PLAYER INPUT FUNCTIONS
//------------------------//

void U3D_MovementComponent::AddHorizontalMovement( FVector direction, float scale, bool isFromPlayerInput )
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
		isFastTurning = false;
		break;
	}
	case MovementState::ATTACKING:
	case MovementState::PREPARING_DROP:
	case MovementState::VERTICAL_DROP:
	{
		if( !isFromPlayerInput )
		{
			addedVelocity = ( scale * direction * horizontalAcceleration ) + addedVelocity;
		}

		isFastTurning = false;
		break;
	}
	case MovementState::NONE:
	{
		isFastTurning = false;
		return;
	}
	default:
	{
		isFastTurning = false;
		return;
	}

	}
}

void U3D_MovementComponent::UpdateTurningVelocity( FVector playerInput, float DeltaTime )
{
	if( maxFastTurnAngle == 0 )
	{
		isFastTurning = false;
		return;
	}

	if( playerInput.IsNearlyZero() || velocity.IsNearlyZero() )
	{
		isFastTurning = false;
		return;
	}

	FVector directionNormalized = FVector( playerInput.X, playerInput.Y, 0 );
	directionNormalized.Normalize();


	// Calculate the angle between the player input and the current velocity.
	float dotProduct = FVector::DotProduct( directionNormalized, velocityxyNormalized );
	float angleDegrees = FMath::RadiansToDegrees( FMath::Acos( dotProduct ) );

	isFastTurning = angleDegrees >= maxFastTurnAngle;

	if( !applyExtraTurnOnFastTurn && isFastTurning )
	{
		return;
	}

	//If the player is not fast turning we check if we need to increase its normal turning speed based on design parameters
	increaseTurningSpeed( angleDegrees, playerInput, DeltaTime );

	return;
}

void U3D_MovementComponent::increaseTurningSpeed( float turnAngle, FVector playerInput, float DeltaTime )
{
	if( turnAngle <= extraTurningSpeedMultiplierAngleLimit )
	{
		return;
	}

	if( FVector( velocity.X, velocity.Y, 0 ).IsNearlyZero() )
	{
		return;
	}

	//Calculate the velocity's perpendicular vector direction
	FVector perpendicular = FVector( -velocity.Y, velocity.X, 0.0f );
	perpendicular.Normalize();

	//Project player input onto the perpendicular vector direction to get the input that will increase the turning speed
	FVector turningSpeed = playerInput.ProjectOnTo( perpendicular );
	turningSpeed.Normalize();

	//Calculate the new input adding increasing the vector that adds turning speed
	FVector inputRedirected = playerInput + ( turningSpeed * ( extraTurningSpeedMultiplier - 1 ) );


	addedVelocity = inputRedirected;
}

void U3D_MovementComponent::JumpStart()
{
	isJumpBtnCurrentlyPressed = true;

	if( !isMovementActive )
	{
		return;
	}

	if( headHit.bBlockingHit )
	{
		return;
	}

	//Check if this input must be saved
	if( isAJumpingState( player->state ) && currentCoyoteTime >= coyoteTime )
	{
		if( jumpBufferTimerHandle.IsValid() )
		{
			jumpBufferTimerHandle.Invalidate();
		}

		GetWorld()->GetTimerManager().SetTimer(
			   jumpBufferTimerHandle, this, &U3D_MovementComponent::timerHandleFinished, jumpBufferDuration, false );

		isJumpSaved = true;

		return;
	}

	if( HeadFreeToJump() )
	{
		if( player->state == MovementState::WALKING )
		{
			initiateJump();
		}
		else if( currentCoyoteTime < coyoteTime )
		{
			initiateJump();
		}
	}
}

bool U3D_MovementComponent::HeadFreeToJump()
{
	FVector sphereCenter = player->GetActorLocation() + FVector( 0, 0,
		player->capsule->GetUnscaledCapsuleHalfHeight() + player->capsule->GetUnscaledCapsuleRadius() - 2 );

	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add( GetOwner() );

	TArray<TEnumAsByte<EObjectTypeQuery>> objects;
	objects.Add( UEngineTypes::ConvertToObjectType( ECollisionChannel::ECC_WorldStatic ) );
	//objects.Add( UEngineTypes::ConvertToObjectType( ECollisionChannel::ECC_WorldDynamic ) );


	TArray<FHitResult> hits;
	bool resul = !UKismetSystemLibrary::SphereTraceMultiForObjects( GetWorld(), sphereCenter, sphereCenter + FVector::UpVector * 8, player->capsule->GetUnscaledCapsuleRadius(), objects, false,
		actorsToIgnore, EDrawDebugTrace::None, hits, true, FLinearColor::Green, FLinearColor::Red, 2 );

	for( FHitResult hit : hits )
	{
		if( hit.bBlockingHit )
		{
			FVector impactPoint = hits[0].ImpactPoint;

			if( GetOwner()->GetActorLocation().X == impactPoint.X &&
			GetOwner()->GetActorLocation().Y == impactPoint.Y )
			{
				return false;
			}
			else
			{
				FVector playerXY = GetOwner()->GetActorLocation();
				playerXY.Z = 0;
				impactPoint.Z = 0;

				FVector headSlide = playerXY - impactPoint;
				FVector normalized = headSlide.GetSafeNormal();
				headSlide += normalized * 10;

				FHitResult newHit;
				GetOwner()->SetActorLocation( GetOwner()->GetActorLocation() + headSlide, false, &newHit );
			}
		}
	}

	return true;
}

void U3D_MovementComponent::JumpStop()
{
	isJumpBtnCurrentlyPressed = false;

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

void U3D_MovementComponent::rotateTowardsVector( float DeltaTime, FVector targetVector )
{
	if( player->state != MovementState::KNOCKBACK )
	{
		if( targetVector.X != 0 || targetVector.Y != 0 )
		{
			FRotator targetRotation = ( FVector( targetVector.X, targetVector.Y, 0 ) ).Rotation();
			FRotator newRotation = FMath::RInterpTo( GetOwner()->GetActorRotation(), targetRotation, DeltaTime, rotatingSpeed );

			GetOwner()->SetActorRotation( newRotation );
		}
	}
}

bool U3D_MovementComponent::castRayToGround( FVector initialLocation, float length, bool longerDebug )
{
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor( GetOwner() );
	
	bool hit = GetWorld()->LineTraceSingleByObjectType( groundedHit, initialLocation, initialLocation + FVector::DownVector * length,
		FCollisionObjectQueryParams( ECC_WorldStatic ), CollisionParams );

	//bool hit = GetWorld()->LineTraceSingleByChannel( groundedHit, initialLocation,
		//initialLocation + FVector::DownVector * length, ECC_WorldStatic, CollisionParams );


		

#if WITH_EDITOR 
	if( groundedRaycast )
	{
		DrawDebugLine( GetWorld(), initialLocation, initialLocation + FVector::DownVector * length,
			hit ? FColor::Green : FColor::Red, false, !hit || longerDebug ? 20: 0, 0U, 5 );
		if( longerDebug )
		{
			DrawDebugLine( GetWorld(), initialLocation, initialLocation + FVector::DownVector * length, FColor::Green, false );
			DrawDebugPoint( GetWorld(), groundedHit.ImpactPoint, 20, FColor::Purple, false, 10 );
		}

		
	}
#endif

	return hit;
}

bool U3D_MovementComponent::setPlayerAtGround()
{
	if( groundedHit.bBlockingHit && player->state != MovementState::DASHING)
	{
		FVector groundLocation = FVector( groundedHit.ImpactPoint.X, groundedHit.ImpactPoint.Y, groundedHit.ImpactPoint.Z );
		TArray<UPrimitiveComponent*> overlapedComps;

		FHitResult hit;

		if( CheckIfLocationIsSafeForPlayer( groundLocation, overlapedComps, true, &hit ) )
		{
			return true;
		}
		else
		{
			if( !velocity.IsNearlyZero() )
			{
				FVector velocityDirection = velocity;
				velocityDirection.Normalize();

				groundLocation = groundLocation + velocityDirection * ( player->capsule->GetUnscaledCapsuleRadius() + 1 );

				return CheckIfLocationIsSafeForPlayer( groundLocation, overlapedComps, true, &hit );
			}
			return false;
		}

	}
	else
	{
		return false;
	}
}

void U3D_MovementComponent::OnMovementCompletlyStops()
{
	player->state = MovementState::NONE;
}

bool U3D_MovementComponent::checkIfPlayerCanBeFixedToGround()
{
	/*if( movementComp->castRayToGround( GetActorLocation() - FVector( 0, 0, capsule->GetUnscaledCapsuleHalfHeight() ), movementComp->maxStepDownHeight ) )
{
	if( movementComp->setPlayerAtGround() )
	{
		state = MovementState::WALKING;
		return;
	}
	else
	{
		movementComp->CheckDownStep();

		if( movementComp->castRayToGround( GetActorLocation() - FVector( 0, 0, capsule->GetUnscaledCapsuleHalfHeight() ), movementComp->maxStepDownHeight ) )
		{
			if( movementComp->setPlayerAtGround() )
			{
				state = MovementState::WALKING;
				return;
			}
		}
	}
}*/

	return false;
}


bool U3D_MovementComponent::CheckJumpBufferAndDoJump( MovementState newState )
{
	if( jumpBufferTimerHandle.IsValid() )
	{
		jumpBufferTimerHandle.Invalidate();
	}

	if( isAJumpingState( player->state ) )
	{
		if( isJumpSaved && newState == MovementState::WALKING )
		{
			player->state = MovementState::WALKING;

			isJumpSaved = false;

			currentCoyoteTime = coyoteTime;
			initiateJump();

			if( !isJumpBtnCurrentlyPressed )
			{
				JumpStop();
			}

			return true;
		}
	}
	else
	{
		isJumpSaved = false;
	}
	return false;
}

float U3D_MovementComponent::GetHitDegreeAngle( FHitResult& hit )
{
	hit.Normal.Normalize();

	return FMath::Abs(
		FMath::Acos( FVector::DotProduct( hit.Normal, FVector::DownVector ) ) *
		180 / PI - 180
	);
}

void U3D_MovementComponent::timerHandleFinished()
{
	isJumpSaved = false;
}

bool U3D_MovementComponent::CheckIfLocationIsSafeForPlayer( FVector location, TArray<UPrimitiveComponent*>& OverlappedComps, bool moveTo, FHitResult* hit )
{
	FVector playerLocation = FVector( location.X, location.Y, location.Z + player->capsule->GetUnscaledCapsuleHalfHeight() + 1 );

	TArray<TEnumAsByte<EObjectTypeQuery>> objects;
	objects.Add( UEngineTypes::ConvertToObjectType( ECollisionChannel::ECC_WorldStatic ) );
	//objects.Add( UEngineTypes::ConvertToObjectType( ECollisionChannel::ECC_WorldDynamic ) );

	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add( GetOwner() );

	UKismetSystemLibrary::CapsuleOverlapComponents( GetWorld(), playerLocation, player->capsule->GetUnscaledCapsuleRadius(), player->capsule->GetUnscaledCapsuleHalfHeight(),
	objects, nullptr, actorsToIgnore, OverlappedComps );



	bool clear = true;
	for( UPrimitiveComponent* comp : OverlappedComps )
	{
		clear = false;
	}

	if( clear )
	{
		float angle = GetHitDegreeAngle( groundedHit );
		if( angle <= maxSlopeAngleDegrees )
		{
			player->SetActorLocation( playerLocation, false, hit );
		}
		else
		{
			clear = false;
		}
	}

#if WITH_EDITOR 
	if( checkIfLocationisSfaeDebug )
	{
		DrawDebugCapsule( GetWorld(), playerLocation, player->capsule->GetUnscaledCapsuleHalfHeight(), player->capsule->GetUnscaledCapsuleRadius(), player->GetActorRotation().Quaternion(), clear ? FColor::Green : FColor::Red, false, 40.f );

		if( !clear )
		{
			DrawDebugLine( GetWorld(), groundedHit.ImpactPoint, groundedHit.Normal * 200, FColor::Orange, false, 20.F, 0U, 1.F );
		}

	}
#endif



	return clear;
}


bool U3D_MovementComponent::isAJumpingState( MovementState stateToCheck )
{
	switch( stateToCheck )
	{
	case MovementState::JUMPING:
	case MovementState::FALLING:
	case MovementState::INAPEX:
		return true;
	default:
		return false;
	}
}

FString U3D_MovementComponent::GetStateAsName( DetectedFloor EnumValue )
{
	const UEnum* EnumPtr = FindObject<UEnum>( ANY_PACKAGE, TEXT( "detectedFloor" ), true );
	if( !EnumPtr ) return FString( "Invalid" );
	return EnumPtr->GetNameByValue( (int64) EnumValue ).ToString();
}


void U3D_MovementComponent::DisableMovementForXSeconds( float seconds, FVector locationToFixPlayer )
{
	
	velocity = FVector::ZeroVector;
	isMovementActive = false;

	if( player->state == MovementState::DASHING )
	{
		player->dashComponent->OnDashFinish();
	}
	player->ChangeMovementState( MovementState::WALKING );
	
	locationToFixPlayer.Z += player->capsule->GetUnscaledCapsuleHalfHeight();
	castRayToGround( locationToFixPlayer, 200 );
	setPlayerAtGround();

	player->ChangeMovementState( MovementState::WALKING );

	FTimerHandle UnusedHandle;
	GetOwner()->GetWorldTimerManager().SetTimer(
		   UnusedHandle, this, &U3D_MovementComponent::EnableMovement, seconds, false );
}

void U3D_MovementComponent::EnableMovement()
{
	isMovementActive = true;
	player->ChangeMovementState( MovementState::WALKING );
}