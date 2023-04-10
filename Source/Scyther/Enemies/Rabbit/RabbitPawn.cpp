// Fill out your copyright notice in the Description page of Project Settings.


#include "RabbitPawn.h"
#include "GameFramework/Character.h"
#include <Kismet/GameplayStatics.h>
#include <Scyther/Components/HealthComponent.h>
#include <Kismet/GameplayStatics.h>

ARabbitPawn::ARabbitPawn() 
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rabbitBody = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "Cuerpo del conejo" ) );
	SetRootComponent( rabbitBody );

	rabbitAttackBox = CreateDefaultSubobject<UBoxComponent>( TEXT( "Caja de ataque" ) );
	rabbitAttackBox->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform, "");

	movementComp = CreateDefaultSubobject<UFloatingPawnMovement>( TEXT( "Floating Movement Comp" ) );
	AddOwnedComponent( movementComp );

	targetMarkComp = CreateDefaultSubobject<UTargetMarkComponent>( TEXT( "Target Mark Comp" ) );
	AddOwnedComponent( targetMarkComp );
}

void ARabbitPawn::BeginPlay()
{
	Super::BeginPlay();

}

void ARabbitPawn::Attack()
{
	if( isAttacking )
	{

	}
	else
	{

		//Play attack animation

		//The attack animation has a frame where the enemy checks if it hits the player
		CheckDamageArea();

		//When the attack animation ends, the attack is reseted to let the enemy attack again



	}
}

void ARabbitPawn::CheckDamageArea(){
	//Check if actor is in hitbox.
	TArray<UPrimitiveComponent*> overlapComps;
	rabbitAttackBox->GetOverlappingComponents( overlapComps );

	for( UPrimitiveComponent* comp : overlapComps )
	{
		if( comp->ComponentHasTag("PlayerBody") )
		{
			isAttacking = true;

			UHealthComponent* health = Cast<UHealthComponent>( UGameplayStatics::GetPlayerPawn( GetWorld(), 0 )
				->GetComponentByClass( UHealthComponent::StaticClass() ) );

			health->receiveDamage( baseDamage, isLightMode ? ElementTypes::LIGHT : ElementTypes::DARK );

			if( attackTimerHandle.IsValid() )
			{
				attackTimerHandle.Invalidate();
			}

			GetWorldTimerManager().SetTimer(
				attackTimerHandle, this, &ARabbitPawn::ResetAttack, 5.f, false );
		}
	}
}

void ARabbitPawn::ResetAttack()
{
	isAttacking = false;
}


void ARabbitPawn::JumpToPosition( FVector targetPosition )
{

}

