// Fill out your copyright notice in the Description page of Project Settings.


#include "improvedRabbit.h"
#include <Components/SphereComponent.h>
#include <Components/BoxComponent.h>
#include <Scyther/ScytherGameModeBase.h>
#include <Scyther/Player/ScytherPlayerPawn.h>
#include <Scyther/Components/HealthComponent.h>
#include <Scyther/Enemies/ImprovedEnemies/Rabbit/improvedRabbitCtrl.h>
#include <GameFramework/FloatingPawnMovement.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <DrawDebugHelpers.h>

bool usePathfinding = false;

AimprovedRabbit::AimprovedRabbit(){

	rabbitRoot = CreateDefaultSubobject<USceneComponent>( TEXT( "Rabbit Root" ) );
	SetRootComponent( rabbitRoot );

	rabbitMesh = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "Rabbit Mesh" ) );
	rabbitMesh->SetupAttachment( RootComponent );

	rabbitHitbox = CreateDefaultSubobject<USphereComponent>( TEXT( "Rabbit hitbox" ) );
	rabbitHitbox->SetupAttachment( rabbitMesh );

	rabbitDamageArea = CreateDefaultSubobject<UBoxComponent>( TEXT( "rabbit damage Area" ) );
	rabbitDamageArea->SetupAttachment( rabbitMesh );

	movementComp = CreateDefaultSubobject<UFloatingPawnMovement>( TEXT( "Floating Movement Comp" ) );
}

void AimprovedRabbit::BeginPlay()
{
	Super::BeginPlay();

	animInstance = rabbitMesh->GetAnimInstance();
	gm = Cast<AScytherGameModeBase>( UGameplayStatics::GetGameMode( GetWorld() ) );
	resetJumpTimerHandle = FTimerHandle();
	healthComp->healthChangedEvent.AddDynamic( this, &AimprovedRabbit::beHit );

	rabbitDamageArea->OnComponentBeginOverlap.AddDynamic( this, &AimprovedRabbit::onAttackAreaOverlap );
}

void AimprovedRabbit::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if( is_AI_Active )
	{
		if( movementComp->MaxSpeed != 0 
			&& (currentAnimation == rabbitAnimations::GoingToPlayer
				|| currentAnimation == rabbitAnimations::RunningAway ))
		{
			FVector newRealMovement = realMovement - GetActorLocation();
			newRealMovement.Normalize();
					
			FRotator newMoveRotator = FMath::RInterpTo( GetActorRotation(), newRealMovement.Rotation(), DeltaTime, rotationSpeed);
			newRealMovement = GetActorLocation() + ( newMoveRotator.Vector() * realMovementMultiplier );
		
			AAIController* aictrl = Cast<AAIController>( GetController() );
			if( aictrl )
			{
				aictrl->MoveToLocation( newRealMovement, 100, true, usePathfinding );
			}

			SetActorRotation( newMoveRotator );
		}
	}
}

void AimprovedRabbit::changeAnimation( rabbitAnimations newAnim )
{
	if( currentAnimation == newAnim )
	{
		return;
	}

	GetWorldTimerManager().ClearTimer( resetJumpTimerHandle );
	resetJumpTimerHandle.Invalidate();

	animInstance->StopAllMontages( 0.25f );
	currentAnimation = newAnim;
	currentJumps = 0;

	isPreparingAttack = true;

	float delayToStart = 0;
	UAnimMontage* animToPlay;

	switch( newAnim )
	{
	case rabbitAnimations::RunningAway:
	{
		movementSpeed = RA_speed;
		maxJumps = RA_maxJumps;
		singleJumpDelay = RA_everyJumpDelay;
		restDelay = RA_allJumpsDelay;

		animToPlay = maxJumps == 1 ? fullJumpMontage : fastJumpMontage;
		break;
	}
	case rabbitAnimations::GoingToPlayer:
	{
		movementSpeed = GP_speed;
		maxJumps = GP_maxJumps;
		singleJumpDelay = GP_everyJumpDelay;
		restDelay = GP_allJumpsDelay;

		animToPlay = maxJumps == 1 ? fullJumpMontage : fastJumpMontage;
		break;
	}
	case rabbitAnimations::NONE:
	{
		return;
		break;
	}
	case rabbitAnimations::HitAnimation:
	{
		animToPlay = hitMontage;
		break;
	}
	case rabbitAnimations::PreparingAttack:
	{
		animToPlay = attackPrepMontage;
		break;
	}
	case rabbitAnimations::Attacking:
	{
		isPreparingAttack = false;
		delayToStart = AT_preparationExtraDelay;
		movementSpeed = AT_speed;

		animToPlay = attackMontage; 
		break;
	}
	default:
	{
		animToPlay = maxJumps == 1 ? fullJumpMontage : fastJumpMontage;
		break;
	}
	}

	if( delayToStart == 0 )
	{
		animInstance->Montage_Play( animToPlay );
	}
	else
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda( [&, animToPlay]()
		{
			animInstance->Montage_Play( animToPlay );
		} );

		GetWorld()->GetTimerManager().SetTimer( resetJumpTimerHandle, TimerDelegate, delayToStart, false );
	}
}

void AimprovedRabbit::flockToThisLocation( FVector newTargetLocation )
{
	newTargetLocation.Z = GetActorLocation().Z;

	FVector traceToTarget = newTargetLocation - GetActorLocation();

	spreadVector = FVector( 0, 0, 0 );

	TArray<AActor*> overlappedActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> objectsType;
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Push( this );

	UKismetSystemLibrary::SphereOverlapActors( GetWorld(), GetActorLocation(), sphereOverlapRadius, objectsType, AimprovedRabbit::StaticClass(), actorsToIgnore, overlappedActors );

	for( AActor* actor : overlappedActors )
	{
		FVector traceV = actor->GetActorLocation() - GetActorLocation();
		float distance = traceV.Size2D();
		float multiplier = FMath::GetMappedRangeValueClamped( FVector2D( 0, maxSpreadRange ), FVector2D( 1, 0 ), distance );

		spreadVector -= traceV * multiplier;
	}

	spreadVector = spreadVector.GetSafeNormal2D() * spreadGrade;
	targetVector = traceToTarget.GetSafeNormal2D() * targetGrade;

	realMovement = GetActorLocation() + ( targetVector + spreadVector ) * realMovementMultiplier;
	realMovement.Z = GetActorLocation().Z;

	AAIController* aictrl = Cast<AAIController>( GetController() );
	if( aictrl )
	{
		aictrl->MoveToLocation( realMovement, 100, true, usePathfinding );
	}
}

void AimprovedRabbit::enableMovement()
{
	movementComp->MaxSpeed = movementSpeed;
	if( currentAnimation == rabbitAnimations::Attacking )
	{
		healthComp->becomeGod();
		enableDamageArea();
	}
}

void AimprovedRabbit::disableMovement( bool fullStop )
{
	movementComp->MaxSpeed = 0;

	if( fullStop )
	{
		movementComp->StopMovementImmediately();
	}

}

void AimprovedRabbit::jumpAnimationFinished()
{
	currentJumps++;

	FTimerDelegate TimerDelegate;
	float delay = singleJumpDelay;

	//We have finished the sequence and need to restart
	if( currentJumps == maxJumps )
	{
		delay = restDelay;
		TimerDelegate.BindLambda( [&]()
		{
			rabbitAnimations previous = currentAnimation;
			changeAnimation( rabbitAnimations::NONE );
			changeAnimation( currentAnimation );
		});
	}
	else
	{
		TimerDelegate.BindLambda( [&]()
		{
			animInstance->Montage_Play( currentJumps < maxJumps - 1 ? fastJumpMontage: fullJumpMontage );
		});
	}

	delay += FMath::RandRange( 0.01f, 0.15f );
	GetWorld()->GetTimerManager().SetTimer( resetJumpTimerHandle, TimerDelegate, delay, false );
}

void AimprovedRabbit::hitAnimationFinished()
{
	changeAnimation( rabbitAnimations::NONE );
}

void AimprovedRabbit::preparationAnimFinished()
{
	isPreparingAttack = false;
}

void AimprovedRabbit::attackAnimationFinished()
{
	canDoDamage = false;
	disableMovement( true );
	changeAnimation( rabbitAnimations::NONE );
	healthComp->becomeMortal();

	AimprovedRabbitCtrl* aictrl = Cast<AimprovedRabbitCtrl>( GetController() );
	if( aictrl )
	{
		aictrl->attackFinished();
	}
}


void AimprovedRabbit::beHit( float oldHp, float newHp, DamageModes damageType )
{
	if( newHp < oldHp )
	{
		canDoDamage = false;
		
		disableMovement( true );


		AimprovedRabbitCtrl* aictrl = Cast<AimprovedRabbitCtrl>( GetController() );
		if( aictrl 
			&& (currentAnimation == rabbitAnimations::PreparingAttack
				|| currentAnimation == rabbitAnimations::Attacking ))
		{
			aictrl->attackInterrupted();
		}
		animInstance->StopAllMontages( 0 );
		changeAnimation( rabbitAnimations::NONE );
		changeAnimation( rabbitAnimations::HitAnimation );
	}
}


void AimprovedRabbit::enableDamageArea()
{
	canDoDamage = true;
	TArray<UPrimitiveComponent*> comps;

	rabbitDamageArea->GetOverlappingComponents( comps );
	for( UPrimitiveComponent* comp : comps )
	{
		if( comp->ComponentHasTag( "PlayerBody" ) )
		{
			damagePlayer( comp->GetOwner() );
		}
	}
}



void AimprovedRabbit::onAttackAreaOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	if( canDoDamage )
	{
		if( OtherComp->ComponentHasTag( "PlayerBody" ) )
		{
			damagePlayer( OtherActor );
		}
	}
}

void AimprovedRabbit::damagePlayer( AActor* actor )
{
	AScytherPlayerPawn* player = Cast<AScytherPlayerPawn>( actor );

	if( player != nullptr )
	{
		canDoDamage = false;

		UHealthComponent* health = Cast<UHealthComponent>( player->GetComponentByClass( UHealthComponent::StaticClass() ) );
		health->receiveDamage( baseDamage );

		//KnockSide player
		FVector playerLocation = player->GetActorLocation();
		playerLocation.Z = 10;

		FVector rabbitLocation = GetActorLocation();
		rabbitLocation.Z = 0;

		//Knock the player a few cm back to not colide with the rabbit collision box
		FVector trace = playerLocation - rabbitLocation;
		trace.Normalize();
		FVector perpendicularTrace = FVector( -trace.Y, trace.X, trace.Z );
		trace = ( trace * attackKnockbackStrenght ) + perpendicularTrace * 2;

		player->ApplyKnockback( trace, 5.f );

		attackAnimationFinished();
	}
}