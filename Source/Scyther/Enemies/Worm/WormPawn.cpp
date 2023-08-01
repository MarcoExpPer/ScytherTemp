// Fill out your copyright notice in the Description page of Project Settings.


#include "WormPawn.h"

#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include <Scyther/Components/HealthComponent.h>
#include "GameFramework/Character.h"
#include "Misc/MapErrors.h"
#include "Scyther/Enemies/BaseEnemyCtrl.h"
#include <Scyther/Player/ScytherPlayerPawn.h>

#include "AIWormCppController.h"
#include "AIServiceWorm/GetIsInside.h"

AWormPawn::AWormPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	bodyWorm = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "Cuerpo gusano" ) );
	SetRootComponent( bodyWorm );

	wormCapsule = CreateDefaultSubobject<UCapsuleComponent>( TEXT( "Caja de recibir y hacer dano" ) );
	wormCapsule->AttachToComponent( bodyWorm, FAttachmentTransformRules::KeepRelativeTransform, "Bone008" );

	/*MainDamageZone = CreateDefaultSubobject<UBoxComponent>( TEXT( "zona vulnerable principal" ) );
	MainDamageZone->AttachToComponent( bodyWorm, FAttachmentTransformRules::KeepRelativeTransform, "Bone008" );

	HeadDamageZone = CreateDefaultSubobject<UBoxComponent>( TEXT( "zona vulnerable cabeza" ) );
	HeadDamageZone->AttachToComponent( bodyWorm, FAttachmentTransformRules::KeepRelativeTransform, "Bone003" );

	TailDamageZone = CreateDefaultSubobject<UBoxComponent>( TEXT( "zona vulnerable cola" ) );
	TailDamageZone->AttachToComponent( bodyWorm, FAttachmentTransformRules::KeepRelativeTransform, "Bone014" );*/

	EffectsPlace = CreateDefaultSubobject<UBoxComponent>( TEXT( "Punto para efectuar los vfx" ) );
	EffectsPlace->AttachToComponent( bodyWorm, FAttachmentTransformRules::KeepRelativeTransform, "" );

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>( TEXT( "Floating Pawn Movement" ) );
	AddOwnedComponent( FloatingPawnMovement );	

}

void AWormPawn::BeginPlay()
{
	Super::BeginPlay();
	//guardo el Scale de cada zona vulnerable definidas en el blueprint para luego cuando quiero que "desaparezcan" cambio su Scale a 0 segun interese
	/*ScaleHeadZoneDefault = HeadDamageZone->K2_GetComponentScale();
	ScaleMainZoneDefault = MainDamageZone->K2_GetComponentScale();
	ScaleTailZoneDefault = TailDamageZone->K2_GetComponentScale();
	ZonesLight();
	turnLightEvent.AddDynamic( this, &AWormPawn::ZonesLight );
	turnDarkEvent.AddDynamic( this, &AWormPawn::ZonesDark );*/
	DefaultMaxSpeed = this->FloatingPawnMovement->GetMaxSpeed();
	MakeInvulnerable();
}

void AWormPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	CheckDamageArea();
}


void AWormPawn::CheckDamageArea()
{
	//comprueba si el gusano ha golpeado al player
	TArray<UPrimitiveComponent*> overlapComps;
	wormCapsule->GetOverlappingComponents( overlapComps );
	for( UPrimitiveComponent* comp : overlapComps )
	{
		if( comp->ComponentHasTag( "PlayerBody" ) && Damage == true )
		{
			Damage = false;

			Cast<AScytherPlayerPawn>( UGameplayStatics::GetPlayerPawn( GetWorld(), 0 ) )->healthComponent->receiveDamage( baseDamage );
			//When the attack animation ends, the attack is reseted to let the enemy attack again

		}
	}
}

void AWormPawn::VerticalAttack()
{

	if( isAttacking )
	{

	}
	else
	{
		isAttacking = true;
		Damage = true;

		//
		//Sonido de crujidos de la tierra cuando el gusano ataca
		if( CrackingWormAttack )
		{
			UFMODBlueprintStatics::PlayEvent2D( GetWorld(), CrackingWormAttack, true ); // 2D Sound
		}

		/*FVector LocationToAttack = this->GetActorLocation();
		LocationToAttack.Z = LocationToAttack.Z - ZToDoAttack;
		FHitResult NotUse;
		this->K2_SetActorLocation( LocationToAttack, false ,NotUse, true);*/
		this->K2_SetActorRotation( FRotator( 0.f, 180.f, 0.f ), true );
		state = AttackType::VERTICAL;
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(
			UnusedHandle, this, &AWormPawn::ResetAttack, Cast<AAIWormCppController>(GetController())->WaitAfterAttack, false );
	}
}

void AWormPawn::HorizontalAttack( FVector RotationEnemy, FVector LocationToAttack )
{
	if( isAttacking )
	{

	}
	else
	{
		isAttacking = true;
		Damage = true;
		if( CrackingWormAttack )
		{
			UFMODBlueprintStatics::PlayEvent2D( GetWorld(), CrackingWormAttack, true ); // 2D Sound
		}

		FRotator EnemyRotation = RotationEnemy.Rotation();
		EnemyRotation.Yaw -= 90.f;
		this->K2_SetActorRotation( EnemyRotation, false );
		FVector fixLocationForAnimation = this->GetActorLocation();
		fixLocationForAnimation.Z += 80.f;
		this->SetActorLocation(fixLocationForAnimation);
		
		state = AttackType::HORIZONTAL;
		FloatingPawnMovement->MaxSpeed = DefaultMaxSpeed;
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(
			UnusedHandle, this, &AWormPawn::ResetAttack, Cast<AAIWormCppController>( GetController() )->WaitAfterAttack, false );


	}
}

void AWormPawn::ZonesLight()
{
	/*HeadDamageZone->SetRelativeScale3D( FVector::ZeroVector );
	MainDamageZone->SetRelativeScale3D( ScaleMainZoneDefault );
	TailDamageZone->SetRelativeScale3D( FVector::ZeroVector );*/
}

void AWormPawn::ZonesDark()
{
	/*HeadDamageZone->SetRelativeScale3D( ScaleHeadZoneDefault );
	MainDamageZone->SetRelativeScale3D( FVector::ZeroVector );
	TailDamageZone->SetRelativeScale3D( ScaleTailZoneDefault );*/
}

void AWormPawn::FinishAttack()
{
	state = AttackType::NOTHING;	

}

void AWormPawn::MakeVulnerable()
{
	healthComp->becomeMortal();
	isHiteable = true;
}

void AWormPawn::MakeInvulnerable()
{
	healthComp->becomeGod();
	isHiteable = false;
}

void AWormPawn::SetZDefault( float NewWormZ )
{
	ZLocationDefault = NewWormZ;

}


void AWormPawn::ResetAttack()
{
	Damage = false;
	isAttacking = false;
}


