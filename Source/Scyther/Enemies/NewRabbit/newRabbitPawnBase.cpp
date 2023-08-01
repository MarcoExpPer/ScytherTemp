// Fill out your copyright notice in the Description page of Project Settings.


#include "newRabbitPawnBase.h"
#include <GameFramework/FloatingPawnMovement.h>
#include "../../Components/TargetMarkComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include <DrawDebugHelpers.h>
#include <Scyther/Player/ScytherPlayerPawn.h>
#include "./newRabbitCtrlBase.h"

AnewRabbitPawnBase::AnewRabbitPawnBase()
{
	rabbitBoxColisionS = CreateDefaultSubobject<USphereComponent>( TEXT( "Rabbit Body Collision S" ) );
	SetRootComponent( rabbitBoxColisionS );
	
	rabbitAttackColisionB = CreateDefaultSubobject<UBoxComponent>( TEXT( "Rabbit attack area Collision B" ) );
	rabbitAttackColisionB->SetupAttachment( RootComponent );

	rabbitSkeleton = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "Rabbit Body Skeletal Mesh" ) );
	rabbitSkeleton->SetupAttachment( RootComponent );
	
	movementComp = CreateDefaultSubobject<UFloatingPawnMovement>( TEXT( "Floating Movement Comp" ) );
}

void AnewRabbitPawnBase::startHopping()
{
	changeRabbitState( RabbitStates::Moving );
	movementComp->MaxSpeed = 0;
}

void AnewRabbitPawnBase::hopPreparationEnds()
{
	float* speedPointer = maxMoveSpeedMap.Find( currentTarget );
	check( speedPointer != nullptr );
	movementComp->MaxSpeed = *speedPointer;
}

void AnewRabbitPawnBase::hopAirEnds()
{
	movementComp->MaxSpeed = 0;

}

void AnewRabbitPawnBase::hopLandEnds()
{
	rabbitToIdle();
}

void AnewRabbitPawnBase::rabbitToIdle()
{
	if( currentState != RabbitStates::Idle )
	{
		changeRabbitState( RabbitStates::Idle );
		stopAIMovement();
	}
}

float AnewRabbitPawnBase::getDelayAfterAllJumps()
{
	float* allJumpDelay = delayAfterAllJumpsMap.Find( currentTarget );
	check( allJumpDelay != nullptr );

	return *allJumpDelay;
}

float AnewRabbitPawnBase::getDelayAfterEveryJump()
{
	float* everyJumpDelay = delayAfterEveryJumpMap.Find( currentTarget );
	check( everyJumpDelay != nullptr );

	return *everyJumpDelay;
}

int AnewRabbitPawnBase::getMaxJumps()
{
	int* jumpsPointer = jumpsMap.Find( currentTarget );
	check( jumpsPointer != nullptr );

	return *jumpsPointer;
}

void AnewRabbitPawnBase::rabbitDoHitMontage()
{
	rabbitSkeleton->GetAnimInstance()->Montage_Play( hitMontage );

	//Change state so the rabbit BTT exists any functionality
	changeRabbitState( RabbitStates::Hit );
}

void AnewRabbitPawnBase::rabbitWasHit( float oldHp, float newHp, DamageModes damageType )
{
	if( newHp < oldHp )
	{
		if( currentState != RabbitStates::Attacking )
		{
			rabbitDoHitMontage();
			stopAIMovement();
		}
	}
}

void AnewRabbitPawnBase::makeRabbitInmortalAndHeal()
{
	healthComp->becomeGod();
	healthComp->changeHealth( healthComp->maxHp, DamageModes::HEALING );
}

void AnewRabbitPawnBase::makeRabbitMortal()
{
	healthComp->becomeMortal();
}

void AnewRabbitPawnBase::setNewTarget( RabbitTargets newTarget )
{
	currentTarget = newTarget;
}

void AnewRabbitPawnBase::toggleBoxDamage( bool activate )
{
	isDamageActive =  activate;

	if( isDamageActive )
	{
		TArray<UPrimitiveComponent*> comps;

		rabbitAttackColisionB->GetOverlappingComponents( comps );
		for( UPrimitiveComponent* comp : comps )
		{
			if( comp->ComponentHasTag( "PlayerBody" ) )
			{
				damagePlayer( comp->GetOwner() );
			}
		}
		
	}
}

void AnewRabbitPawnBase::startAttackPreparation()
{
	changeRabbitState( RabbitStates::PreparingAttack );
	movementComp->MaxSpeed = 0;
}

void AnewRabbitPawnBase::preparationFinished()
{
	changeRabbitState( RabbitStates::AttackReady );
}

void AnewRabbitPawnBase::doAttack()
{
	changeRabbitState( RabbitStates::Attacking );
	movementComp->MaxSpeed = attackMaxMoveSpeed;
}

void AnewRabbitPawnBase::attackFinished()
{
	changeRabbitState( RabbitStates::AttackFinished );
	stopAIMovement();
}

void AnewRabbitPawnBase::BeginPlay()
{
	Super::BeginPlay();
	//Fill all maps to make the code clearer and remove possible switches

	//ToPlayer
	jumpsMap.Add( RabbitTargets::ToPlayer, goToPlayerJumps );
	delayAfterAllJumpsMap.Add( RabbitTargets::ToPlayer, goToPlayerDelayAfterAllJumps );
	delayAfterEveryJumpMap.Add( RabbitTargets::ToPlayer, goToPlayerDelayAfterEveryJump );
	maxMoveSpeedMap.Add( RabbitTargets::ToPlayer, goToPlayerMaxMoveSpeed );

	//Return Home
	jumpsMap.Add( RabbitTargets::ReturnHome, returnHomeJumps );
	delayAfterAllJumpsMap.Add( RabbitTargets::ReturnHome, returnHomeDelayAfterAllJumps );
	delayAfterEveryJumpMap.Add( RabbitTargets::ReturnHome, returnHomeDelayAfterEveryJump );
	maxMoveSpeedMap.Add( RabbitTargets::ReturnHome, returnHomeMaxMoveSpeed );

	//Run Away
	jumpsMap.Add( RabbitTargets::RunAway, runAwayJumps );
	delayAfterAllJumpsMap.Add( RabbitTargets::RunAway, runAwayDelayAfterAllJumps );
	delayAfterEveryJumpMap.Add( RabbitTargets::RunAway, runAwayDelayAfterEveryJump );
	maxMoveSpeedMap.Add( RabbitTargets::RunAway, runAwayMaxMoveSpeed );

	//GoToLocation
	jumpsMap.Add( RabbitTargets::GoToLocation, runAwayJumps );
	delayAfterAllJumpsMap.Add( RabbitTargets::GoToLocation, runAwayDelayAfterAllJumps );
	delayAfterEveryJumpMap.Add( RabbitTargets::GoToLocation, runAwayDelayAfterEveryJump );
	maxMoveSpeedMap.Add( RabbitTargets::GoToLocation, runAwayMaxMoveSpeed );

	//Delegates
	healthComp->healthChangedEvent.AddDynamic( this, &AnewRabbitPawnBase::rabbitWasHit );

	rabbitAttackColisionB->OnComponentBeginOverlap.AddDynamic( this, &AnewRabbitPawnBase::OnBoxCollisionOverlap );
	rabbitAttackColisionB->OnComponentHit.AddDynamic( this, &AnewRabbitPawnBase::OnBoxHit );
	rabbitBoxColisionS->OnComponentHit.AddDynamic( this, &AnewRabbitPawnBase::OnBoxHit );

	initialTransform.SetLocation(  GetActorLocation());
}

void AnewRabbitPawnBase::stopAIMovement()
{
	//Stop moveTo task without making it fail
	UBlackboardComponent* BlackboardComp = Cast<AnewRabbitCtrlBase>( GetController() )->GetBlackboardComponent();
	BlackboardComp->SetValueAsVector( "TargetLocation", GetActorLocation() );

	//Stop movement
	movementComp->MaxSpeed = 0;
	movementComp->StopActiveMovement();
}

void AnewRabbitPawnBase::OnBoxHit( UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit )
{
	if( isDamageActive )
	{
		if( OtherComp->ComponentHasTag( "PlayerBody" ) )
		{
			damagePlayer( OtherActor );
		}
	}
}
void AnewRabbitPawnBase::OnBoxCollisionOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	if( isDamageActive )
	{
		if( OtherComp->ComponentHasTag( "PlayerBody" ) )
		{
			damagePlayer( OtherActor );
		}
	}
}

void AnewRabbitPawnBase::damagePlayer(AActor* pl)
{
	AScytherPlayerPawn* player = Cast<AScytherPlayerPawn>( pl ); 
	
	UHealthComponent* health = Cast<UHealthComponent>( player->GetComponentByClass( UHealthComponent::StaticClass() ) );
	health->receiveDamage( attackDamage, damageMode );

	isDamageActive = false;
	stopAIMovement();

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

	attackFinished();
}
void AnewRabbitPawnBase::changeRabbitState( RabbitStates newState )
{
	//Si esta pasando a modo atacar, se hace inmortal
	if( newState == RabbitStates::Attacking )
	{
		healthComp->becomeGod();
	}//Si esta abandonando el modo atacar, se hace mortal
	else if( currentState == RabbitStates::Attacking )
	{
		healthComp->becomeMortal();
	}

	currentState = newState;
}