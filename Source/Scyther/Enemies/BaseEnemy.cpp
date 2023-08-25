// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include <Scyther/ScytherGameModeBase.h>
#include <Kismet/GameplayStatics.h>
#include "AIModule/Classes/BrainComponent.h"
#include <Scyther/Components/AttackComponent.h>
#include <Scyther/Components/DropLooteablesComponent.h>
#include <Scyther/Components/HealthComponent.h>
#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "Scyther/Player/ScytherPlayerPawn.h"
#include "Scyther/Player/RespawnManager.h"
#include "Scyther/Items/CombatZone/CombatRoundPoint.h"
#include <Scyther/Enemies/ImprovedEnemies/Worm/newWorm.h>
#include "Worm/WormPawn.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	healthComp = CreateDefaultSubobject<UHealthComponent>( TEXT( "Mycomp" ) );
	dropLooteablesComp = CreateDefaultSubobject<UDropLooteablesComponent>( TEXT( "LootComp" ) );

	targetMarkComp = CreateDefaultSubobject<UTargetMarkComponent>( TEXT( "Target Mark Comp" ) );
	AddOwnedComponent( targetMarkComp );
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	initialTransform = GetActorTransform();

	isInLightArea = isLightMode;
	originalLightMode = isLightMode;

	healthComp->zerohealthEvent.AddDynamic( this, &ABaseEnemy::whenHpGoesTo0 );

	auto* pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	AScytherPlayerPawn* character = Cast<AScytherPlayerPawn>( pawn );
	UAttackComponent* attackComp = (UAttackComponent*) character->GetComponentByClass( UAttackComponent::StaticClass() );
	attackComp->hittedEnemyEvent.AddDynamic( this, &ABaseEnemy::WhenIsHitted );
}

// Called every frame
void ABaseEnemy::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if( isTransforming )
	{
		currentTransformationTimer += DeltaTime;

		if( !isLightMode && currentTransformationTimer >= maxShadowToLightTimer )
		{
			turnLightMode();
			return;
		}

		if( isLightMode && currentTransformationTimer >= maxLightToShadowTimer )
		{
			turnDarkMode();
			return;
		}
	}
	if( isHitted )
	{
		KnockbackWhenIsHitted( DeltaTime );
	}
}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent( UInputComponent* PlayerInputComponent )
{
	Super::SetupPlayerInputComponent( PlayerInputComponent );

}

void ABaseEnemy::turnLightMode()
{
	isLightMode = true;
	isTransforming = false;
	turnLightEvent.Broadcast();
}


void ABaseEnemy::turnDarkMode()
{
	isLightMode = false;
	isTransforming = false;
	turnDarkEvent.Broadcast();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation( this, ChangeToDarkVFX, this->GetActorLocation(), FRotator::ZeroRotator, FVector( 1.f ) );
}


void ABaseEnemy::GenerateLoot()
{
	AWormPawn* wormPawn = Cast<AWormPawn>( this );

	if( wormPawn )
	{
		dropLooteablesComp->DropLooteables( FVector( wormPawn->wormCapsule->GetComponentLocation().X, wormPawn->wormCapsule->GetComponentLocation().Y, ( UGameplayStatics::GetPlayerPawn( GetWorld(), 0 ) )->GetActorLocation().Z ) , isLightMode );

	}
	else
	{
		dropLooteablesComp->DropLooteables( GetActorLocation(), isLightMode );
	}
}

void ABaseEnemy::whenHpGoesTo0( DamageModes type )
{
	if( !isDead )
	{
		isDead = true;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation( this, DeadVFX, this->GetActorLocation(), FRotator::ZeroRotator, FVector( 1.f ) );

		if( isLightMode )
		{
			dropLooteablesComp->manaCrystalsAmount = lightManaCrystals;
			dropLooteablesComp->lightAmount = lightManaAmount;

			dropLooteablesComp->healthCrystalAmount = 0;
			dropLooteablesComp->healthAmount = 0;
		}
		else
		{
			float rnd = FMath::FRand();

			if( rnd <= healthChance )
			{
				dropLooteablesComp->manaCrystalsAmount = 0;
				dropLooteablesComp->lightAmount = 0;

				dropLooteablesComp->healthCrystalAmount = healCrystals;
				dropLooteablesComp->healthAmount = healAmount;
			}
			else
			{
				dropLooteablesComp->manaCrystalsAmount = lightManaCrystals;
				dropLooteablesComp->lightAmount = lightManaAmount / 2;

				dropLooteablesComp->healthCrystalAmount = 0;
				dropLooteablesComp->healthAmount = 0;
			}
		}

		GenerateLoot();

		if( ThisRound != nullptr )
		{
			ThisRound->SingleEnemyDead();
		}

		deSpawn();
	}
}

void ABaseEnemy::spawn( FTransform spawnTransform )
{
	SetActorTransform( spawnTransform );

	if(this->GetOwner()->GetClass() != AWormPawn::StaticClass() && this->GetOwner()->GetClass() != AnewWorm::StaticClass() )
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation( this, isLightMode ? LightSpawnVFX : DarkSpawnVFX, this->GetActorLocation(), FRotator::ZeroRotator, FVector( 1.f ) );

	}

	SetActorHiddenInGame( false );
	healthComp->changeHealth( this->healthComp->initialHp, DamageModes::HEALING );

}

void ABaseEnemy::deSpawn()
{
	if( changeToDarkTimerHandle.IsValid() )
	{
		GetWorldTimerManager().ClearTimer( changeToDarkTimerHandle );
		changeToDarkTimerHandle.Invalidate();
	}

	if( originalLightMode != isLightMode )
	{
		if( originalLightMode )
		{
			turnLightMode();
		}
		else
		{
			turnDarkMode();
		}
		isLightMode = originalLightMode;
	}

	SetActorHiddenInGame( true );

	// Disables collision components
	SetActorEnableCollision( false );

	toggleAI( false );

}



//INTERFACES

void ABaseEnemy::DoDirectDamage_Implementation( float damage )
{
	healthComp->receiveDamage( damage );
}


void ABaseEnemy::EnterDarkArea_Implementation()
{
	isInLightArea = false;

	if( isLightMode && !isAfectedBySkill )
	{
		isTransforming = true;
		currentTransformationTimer = 0;
	}
}
void ABaseEnemy::EnterLightArea_Implementation()
{
	isInLightArea = true;

	if( !isLightMode && !isAfectedBySkill )
	{
		isTransforming = true;
		currentTransformationTimer = 0;
	}
}

void ABaseEnemy::SkillToLightMode_Implementation()
{
	isAfectedBySkill = true;
	this->turnLightMode();

}
void ABaseEnemy::SkillToDarkMode_Implementation()
{
	isAfectedBySkill = true;
	this->turnDarkMode();
}

void ABaseEnemy::SkillEnds_Implementation()
{
	isAfectedBySkill = false;

	if( isInLightArea )
	{
		this->EnterLightArea_Implementation();
	}
	else
	{
		this->EnterDarkArea_Implementation();
	}
}

void ABaseEnemy::WhenIsHitted( FString name, FVector characterLocation, AttackState typeAttack )
{

	bool* isInmune = healthComp->inmunities.Find( DamageModes::DASHEABLE );
	if( isInmune == nullptr )
	{
		isInmune = false;
	}
	
	if( isInmune )
	{
		return;
	}


	if( GetName().Equals( name ) )
	{
		characterLocationWhenReceiveHit = characterLocation;
		typeAttackReceived = typeAttack;
		CalculateTargetPositionWhenIsHitted();
	}

}

void ABaseEnemy::bindToRoundStart( ACombatRoundPoint* round )
{

	ThisRound = round;

	round->CombatRoundActivated.AddDynamic( this, &ABaseEnemy::roundStartEventExecution );
}

void ABaseEnemy::toggleAI(bool activate)
{
	is_AI_Active = activate;
	isDead = !activate;

	// Stops the Actor from ticking
	SetActorTickEnabled( activate );

	// Disables collision components
	SetActorEnableCollision( activate );

	// Stops the Actor from ticking
	SetActorTickEnabled( activate );
	ABaseEnemyCtrl* baseCtrl = Cast<ABaseEnemyCtrl>( GetController() );
	if( baseCtrl != nullptr && baseCtrl->BrainComponent != nullptr )
	{
		if( !activate )
		{
			if( UWorld* World = GetWorld() )
			{
				auto respawnerClass = UGameplayStatics::GetActorOfClass( World, ARespawnManager::StaticClass() );
				ARespawnManager* RespawnManager = Cast<ARespawnManager>( respawnerClass );
				RespawnManager->DeadEnemies.Add( baseCtrl );
			}
		}
	}
}

void ABaseEnemy::roundStartEventExecution()
{
	if( changeToDarkModeAfterSomeTime )
	{
		turningDarkEvent.Broadcast( timeToChangeToDark );

		GetWorldTimerManager().SetTimer(
		   changeToDarkTimerHandle, this, &ABaseEnemy::turnDarkMode, timeToChangeToDark, false );
	}

	toggleAI( true );
}

void ABaseEnemy::CalculateTargetPositionWhenIsHitted()
{
	float force = 0.f;
	switch( typeAttackReceived )
	{
	case AttackState::FIRST:
		force = firstAttackForceKnockback;
		break;
	case AttackState::SECOND:
		force = secondAttackForceKnockback;
		break;
	case AttackState::THIRD:
		force = finalAttackForceKnockback;
		break;
	case AttackState::AIR:
		force = airAttackForceKnockback;
		break;
	case AttackState::BODY:
		force = airAttackBodyHitForceKnockback;
		break;
	default:
		break;
	}
	FVector directionVec = GetActorLocation() - characterLocationWhenReceiveHit;
	directionVec.Z = ( directionVec.Z <= 0 ) ? directionVec.Z * -1 : directionVec.Z;
	directionVec.Z = FMath::Clamp( directionVec.Z, 0.f, 10.f );
	directionVec.Normalize();
	isHitted = force != 0;
	targetLocationWhenIsHitted = GetActorLocation() + directionVec * force;
}

void ABaseEnemy::KnockbackWhenIsHitted( float deltaTime )
{
	FVector directionVec = GetActorLocation() - characterLocationWhenReceiveHit;
	directionVec.Z = ( directionVec.Z <= 0 ) ? directionVec.Z * -1 : directionVec.Z;
	directionVec.Z = FMath::Clamp( directionVec.Z, 0.f, 10.f );
	directionVec.Normalize();
	FVector newPosition = GetActorLocation() + directionVec * deltaTime * speedKnockback;
	SetActorLocation( newPosition, true );
	float size = ( targetLocationWhenIsHitted - GetActorLocation() ).Size();
	if( size > 128.f )
	{
		isHitted = false;
	}
}

bool ABaseEnemy::GetIsDead()
{
	return isDead;
}

bool ABaseEnemy::GetIsInCombatZone()
{
	return isInCombatZone;
}

float ABaseEnemy::getSqrDistanceToLocation(FVector location, bool ignoreZ) const
{
	FVector selfLocation = GetActorLocation();

	if (ignoreZ) {
		selfLocation.Z = 0;
	}


	return FVector::DistSquared(selfLocation, location);
}