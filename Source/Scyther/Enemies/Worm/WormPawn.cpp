// Fill out your copyright notice in the Description page of Project Settings.


#include "WormPawn.h"

#include "Kismet/GameplayStatics.h"
#include <Scyther/Components/HealthComponent.h>

#include "GameFramework/Character.h"
#include "Misc/MapErrors.h"
#include "Scyther/Enemies/BaseEnemyCtrl.h"
#include <Scyther/Player/ScytherPlayerPawn.h>

AWormPawn::AWormPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	wormBody = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "Cuerpo del gusano" ) );
	SetRootComponent( wormBody );

	wormBodyReal = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "Cuerpo real del gusano" ) );
	wormBodyReal->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform, "body" );

	wormAttackCapsule = CreateDefaultSubobject<UCapsuleComponent>( TEXT( "Caja de ataque2" ) );
	wormAttackCapsule->AttachToComponent( wormBodyReal, FAttachmentTransformRules::KeepRelativeTransform, "capsule");

	MainDamageZone = CreateDefaultSubobject<UBoxComponent>( TEXT( "zona vulnerable principal" ) );
	MainDamageZone->AttachToComponent( wormBodyReal, FAttachmentTransformRules::KeepRelativeTransform, "Main Zone" );

	targetMarkComp = CreateDefaultSubobject<UTargetMarkComponent>( TEXT( "Target Mark Comp" ) );
	AddOwnedComponent( targetMarkComp );

	
}

void AWormPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckDamageArea();
}


void AWormPawn::CheckDamageArea()
{

	TArray<UPrimitiveComponent*> overlapComps;
	wormAttackCapsule->GetOverlappingComponents( overlapComps );
	for( UPrimitiveComponent* comp : overlapComps )
	{
		if( comp->ComponentHasTag( "PlayerBody" ) && Damage==true)
		{
			Damage = false;

			Cast<AScytherPlayerPawn>( UGameplayStatics::GetPlayerPawn( GetWorld(), 0 ) )->healthComponent-> receiveDamage( baseDamage );
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
		UFMODBlueprintStatics::PlayEventAtLocation( GetWorld(), CrackingWormAttack, GetActorTransform(), true );

		//Audio Rugido del gusano y Crujidos de tierra
		UFMODBlueprintStatics::PlayEventAtLocation( GetWorld(), RoarWormVerticalAttack, GetActorTransform(), true );

		//
		wormBody->AddImpulse( *( new FVector( 0, 0, 1000 ) ), "", true );
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(
			UnusedHandle, this, &AWormPawn::ResetAttack, 4.f, false );
	}
}

void AWormPawn::HorizontalAttack(  FVector EnemyPosition )
{
	if( isAttacking )
	{

	}
	else
	{
		float XEnemy = EnemyPosition.X * -1.f;
		float YEnemy = EnemyPosition.Y * -1.f;
		isAttacking = true;
		Damage = true;

		//Sonido de crujidos de la tierra cuando el gusano ataca
		UFMODBlueprintStatics::PlayEventAtLocation( GetWorld(), CrackingWormAttack, GetActorTransform(), true );

		//Audio Rugido del gusano para el ataque horizontal del gusano
		UFMODBlueprintStatics::PlayEventAtLocation( GetWorld(), RoarWormHorizontalAttack, GetActorTransform(), true );

		//

		wormBody->AddImpulse( *( new FVector( XEnemy*1.7f, YEnemy*1.7f, 650 ) ), "", true );

		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(
			UnusedHandle, this, &AWormPawn::ResetAttack, 4.f, false );
	}
}

void AWormPawn::ResetAttack()
{
	Damage = false;
	isAttacking = false;
}


