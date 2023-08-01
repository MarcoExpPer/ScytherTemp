// Fill out your copyright notice in the Description page of Project Settings.


#include "EntityShadow.h"
#include <Scyther/Items/LootableItem.h>

// Sets default values for this component's properties
UEntityShadow::UEntityShadow()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UEntityShadow::BeginPlay()
{
	Super::BeginPlay();

	PrimaryComponentTick.bCanEverTick = doTick;

	character = Cast<AScytherPlayerPawn>( GetOwner() );
	if( !character )
	{
		lootableItem = Cast<ALootableItem>( GetOwner() );

		if( !lootableItem )
		{
			UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra la sombra del jugador" ) );
			return;
		}
		entityShadow = lootableItem->entityShadow;
	}
	else
	{
		entityShadow = character->entityShadow;
		circleRingShadow = character->circleRingShadow;
	}

	
	
	if( !entityShadow )
	{
		if( entityShadow )
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra la sombra del jugador" ) );
		return;
	}	
	
	if( !circleRingShadow )
	{
		if( circleRingShadow )
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra el indicador circular del jugador" ) );
		return;
	}


	TPtoFloor();
}


// Called every frame
void UEntityShadow::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TPtoFloor();
}

void UEntityShadow::TPtoFloor()
{
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor( GetOwner() );

	FHitResult groundedHit;

	if( GetWorld()->LineTraceSingleByChannel( groundedHit, GetOwner()->GetActorLocation(),
		GetOwner()->GetActorLocation() + FVector::DownVector * 10000, ECC_WorldStatic, CollisionParams ) )
	{
		FHitResult hit;
		if( circleRingShadow )
		{
			circleRingShadow->K2_SetWorldLocation( groundedHit.ImpactPoint, false, hit, true );
			entityShadow->K2_SetWorldLocation( groundedHit.ImpactPoint + FVector::FVector(0.f, 1.f, 0.f), false, hit, true);
		}
		else
		{
			entityShadow->K2_SetWorldLocation( groundedHit.ImpactPoint, false, hit, true );
		}
		
	}
}
