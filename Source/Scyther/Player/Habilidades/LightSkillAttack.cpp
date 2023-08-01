// Fill out your copyright notice in the Description page of Project Settings.


#include "LightSkillAttack.h"
#include "../../Components/AttackComponent.h"
#include "../../Components/HealthComponent.h"
#include "../../Components/SkillComponent.h"


// Sets default values
ALightSkillAttack::ALightSkillAttack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Instanciar la caja de colisiones
	boxCollision = CreateDefaultSubobject<UBoxComponent>( TEXT( "Box Collision" ) );
	SetRootComponent( boxCollision );
	//boxCollision->SetupAttachment( RootComponent );
	boxCollision->SetGenerateOverlapEvents( true );
	//boxCollision->SetHiddenInGame( false );
	boxCollision->OnComponentBeginOverlap.AddDynamic( this, &ALightSkillAttack::BeginOverlap );

	//NiagaraSystem
	niagaraSystem = CreateDefaultSubobject<UNiagaraComponent>( TEXT( "NiagaraSystem" ) );
	niagaraSystem->SetupAttachment( RootComponent );

	//Valores por defecto para las variables;
	effectDirection = FVector(0, 0, 0);
	effectDuration = 1.f;
	actualEffectDuration = 1.f;
	effectDamage = 10.f;
	effectWidth = 50.f;
	effectSpeed = 15.f;
	skillVFX = nullptr;
}

// Called when the game starts or when spawned
void ALightSkillAttack::BeginPlay()
{
	Super::BeginPlay();

	character = (AScytherPlayerPawn*) UGameplayStatics::GetPlayerPawn( GetWorld(), 0 );

}

// Called every frame
void ALightSkillAttack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->SetActorLocation( GetActorLocation() + ( DeltaTime * effectSpeed * effectDirection ) );

	if( actualEffectDuration >= 0 )
	{
		actualEffectDuration -= DeltaTime;
	}
	else
	{
		this->Destroy();
	}

}


// Overlap
void ALightSkillAttack::BeginOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	if( !damagedEnemies.Contains( OtherActor->GetName() ) )
	{
		if( isADamageableEnemyComp( OtherActor, OtherComp ) )
		{
			UE_LOG( LogTemp, Warning, TEXT( "La habilidad ha golpeado %s" ), *OtherActor->GetName() );
			UHealthComponent* healthComp = (UHealthComponent*) OtherActor->GetComponentByClass( UHealthComponent::StaticClass() );
			if( healthComp )
			{
				healthComp->receiveDamage( effectDamage, DamageModes::INEVITABLE );
			}
			GetImpactPointHitToVFX( OverlappedComponent, OtherComp );
			damagedEnemies.Add( OtherActor->GetName() );
		}
		else if( isADamageableDestructible( OtherActor ) )
		{
			UE_LOG( LogTemp, Warning, TEXT( "La habilidad ha golpeado %s" ), *OtherActor->GetName() );
			UHealthComponent* healthComp = (UHealthComponent*) OtherActor->GetComponentByClass( UHealthComponent::StaticClass() );
			if( healthComp )
			{
				healthComp->receiveDamage( effectDamage, DamageModes::INEVITABLE );
			}
			GetImpactPointHitToVFX( OverlappedComponent, OtherComp );
			damagedEnemies.Add( OtherActor->GetName() );
		}
	}
	
}


void ALightSkillAttack::InitLightSkillAttackValues( FVector newDir, float newDuration, float newDamage, float newWidth, float newHeight, float newSpeed, UNiagaraSystem* newVFX, bool isSpecial )
{
	effectDirection = newDir;
	
	effectDuration = newDuration;
	actualEffectDuration = newDuration;

	effectDamage = newDamage;

	effectWidth = newWidth;
	effectHeight = newHeight;
	boxCollision->SetBoxExtent( FVector( 100, effectWidth, effectHeight ) );

	effectSpeed = newSpeed;

	skillVFX = newVFX;

	if( isSpecial )
	{
		this->AddActorLocalRotation( FRotator( 0, 0, 90 ) );
	}
}


void ALightSkillAttack::StartVFX()
{
	niagaraSystem->SetAsset( skillVFX );
	niagaraSystem->SetNiagaraVariableFloat( FString( "User.Duration" ), effectDuration );
	niagaraSystem->SetNiagaraVariableFloat( FString( "User.ArcWidth" ), effectWidth );
	niagaraSystem->SetNiagaraVariableFloat( FString( "User.Speed" ), effectSpeed );
}

void ALightSkillAttack::GetImpactPointHitToVFX( UPrimitiveComponent* OtherActor, UPrimitiveComponent* OtherComp )
{
	if( !character->skillComponent->lightSkillFlashHitVFX )
	{
		UE_LOG( LogTemp, Warning, TEXT( "No tiene VFX de golpeo de hablidad de luz asignado el personaje." ) );
		return;
	}
	if( OtherActor && OtherComp )
	{
		TArray<FHitResult> Results;
		auto Start = OtherActor->GetComponentLocation();
		auto End = OtherComp->GetComponentLocation();
		auto CollisionRadius = FVector::Dist( Start, End ) * 1.2f;

		// spherical sweep 
		GetWorld()->SweepMultiByObjectType( Results, Start, End,
			FQuat::Identity, 0,
			FCollisionShape::MakeSphere( CollisionRadius ),
			// use custom params to reduce the search space
			FCollisionQueryParams::FCollisionQueryParams( FName( "VFXimpact" ), false, nullptr )
		);

		for( auto HitResult : Results )
		{
			AActor* hitActor = HitResult.GetActor();

			if( hitActor == nullptr )
			{
				break;
			}

			if( OtherComp->GetUniqueID() == HitResult.GetComponent()->GetUniqueID() )
			{

				UNiagaraFunctionLibrary::SpawnSystemAtLocation( GetWorld(), character->skillComponent->lightSkillFlashHitVFX, HitResult.ImpactPoint );
			}
		}
	}
}


bool ALightSkillAttack::isADamageableEnemyComp( AActor* actor, UPrimitiveComponent* componentHit = nullptr )
{
	if( componentHit->ComponentHasTag( "EnemyBody" ) )
	{
		if( ( (ABaseEnemy*) actor )->isAfectedByLightSkill )
		{
			return true;
		}
	}

	return false;
}

bool ALightSkillAttack::isADamageableDestructible( AActor* actor )
{
	if( actor->GetClass()->IsChildOf( character->attackComponent->DestructibleClass ) )
	{
		return true;
	}

	if( actor->GetClass()->IsChildOf( character->attackComponent->SecondDestructibleClass ) )
	{
		return true;
	}

	return false;
}