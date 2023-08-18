// Fill out your copyright notice in the Description page of Project Settings.


#include "newWorm.h"
#include "Components/SkeletalMeshComponent.h"
#include <GameFramework/FloatingPawnMovement.h>
#include <Scyther/Player/ScytherPlayerPawn.h>
#include <Scyther/Components/HealthComponent.h>

AnewWorm::AnewWorm()
{
	wormRoot = CreateDefaultSubobject<USceneComponent>( TEXT( "Worm root" ) );
	SetRootComponent( wormRoot );

	wormMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Worm Mesh"));
	wormMesh->SetupAttachment( RootComponent );

	movementComp = CreateDefaultSubobject<UFloatingPawnMovement>( TEXT( "Floating Movement Comp" ) );
}

void AnewWorm::BeginPlay()
{
	Super::BeginPlay();
	//Fill all maps to make the code clearer and remove possible switches
	animInstance = wormMesh->GetAnimInstance();

	wormMesh->OnComponentBeginOverlap.AddDynamic( this, &AnewWorm::OnMeshOverlap );
	
	initialTransform.SetLocation( GetActorLocation() );
}

void AnewWorm::toUndergroundMode()
{
	wormMesh->SetVisibility( false, false );
	disableDamageArea();
}

void AnewWorm::toOverworldMode()
{
	wormMesh->SetVisibility( true, false );
	enableDamageArea();
}

void AnewWorm::enableDamageArea()
{
	isDamageAreaActive = true;

	TArray<UPrimitiveComponent*> comps;
	wormMesh->GetOverlappingComponents( comps );

	for( UPrimitiveComponent* comp : comps)
	{
		checkComponent( comp );
	}
}

void AnewWorm::disableDamageArea()
{
	isDamageAreaActive = false;
}

float AnewWorm::doVerticalAttack()
{
	animInstance->Montage_Play( verticalAttackMontage );
	
	return verticalAttackMontage->SequenceLength;
}

float AnewWorm::doHorizontalAttack()
{
	animInstance->Montage_Play( horizontalAttackMontage );
	return verticalAttackMontage->GetPlayLength();
}

void AnewWorm::OnMeshOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	checkComponent( OtherComp );
}

void AnewWorm::checkComponent( UPrimitiveComponent* compToCheck )
{
	if( isDamageAreaActive && compToCheck->ComponentHasTag( "PlayerBody" ) )
	{
		isDamageAreaActive = false;
	
		AScytherPlayerPawn* pawn = Cast< AScytherPlayerPawn>( compToCheck->GetOwner() );
		pawn->healthComponent->receiveDamage( baseDamage );
	}
}