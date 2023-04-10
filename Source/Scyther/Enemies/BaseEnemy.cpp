// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include <Scyther/ScytherGameModeBase.h>
#include <Kismet/GameplayStatics.h>
#include "AIModule/Classes/BrainComponent.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	healthComp = CreateDefaultSubobject<UHealthComponent>( TEXT( "Mycomp" ) );
	AddOwnedComponent( healthComp );

	
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	isInLightArea = isLightMode;

	//baseCtrl = Cast<ABaseEnemyCtrl>(GetController());
	healthComp->zerohealthEvent.AddDynamic( this, &ABaseEnemy::whenHpGoesTo0 );
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
}


void ABaseEnemy::GenerateLoot()
{
	AScytherGameModeBase * gm = Cast<AScytherGameModeBase>( UGameplayStatics::GetGameMode( GetWorld() ) );

	float manaAmount = isLightMode ? lightDrops / crystalsAmount : darkDrops / crystalsAmount;
	float expAmount = experiencePoints / crystalsAmount;

	for( int i = 0; i < crystalsAmount; ++i )
	{
		FVector loc = GetActorLocation();

		loc.X = loc.X + FMath::FRandRange( -100, 100 );
		loc.Y = loc.Y + FMath::FRandRange( -100, 100 );
		loc.Z = loc.Z + FMath::FRandRange( -20, 20 );

		gm->manaPool->spawnDrop( loc, isLightMode, manaAmount, expAmount );
	}
}


void ABaseEnemy::whenHpGoesTo0( ElementTypes type )
{
	if( !isDead )
	{
		isDead = true;
		//GenerateLoot();
		deSpawn();
	}
}

void ABaseEnemy::spawn( FVector spawnPosition )
{
#if WITH_EDITOR 
	UE_LOG( LogTemp, Error, TEXT( "ABaseEnemy spawn function is being executed and is not implemented" ) );
	GEngine->AddOnScreenDebugMessage( -1, 4, FColor::Red, FString::Printf( TEXT( "ABaseEnemy spawn function is being executed and is not implemented" ) ) );
#endif
}
void ABaseEnemy::deSpawn()
{
	SetActorHiddenInGame( true );

	// Disables collision components
	SetActorEnableCollision( false );

	// Stops the Actor from ticking
	SetActorTickEnabled( false );
	ABaseEnemyCtrl* baseCtrl = Cast<ABaseEnemyCtrl>( GetController() );
	if( baseCtrl != nullptr )
	{
		baseCtrl->BrainComponent->StopLogic( "" );
	}
		
}



//INTERFACES

void ABaseEnemy::DoDirectDamage_Implementation( float damage )
{
	healthComp->receiveDamage( damage );
}


void ABaseEnemy::EnterDarkArea_Implementation()
{	
	isInLightArea = false;

	if( isLightMode && !isAfectedBySkill)
	{
		isTransforming = true;
		currentTransformationTimer = 0;
	}
}
void ABaseEnemy::EnterLightArea_Implementation( )
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

bool ABaseEnemy::GetIsDead()
{
	return isDead;
}
