// Fill out your copyright notice in the Description page of Project Settings.


#include "ManadropPool.h"


// Called when the game starts or when spawned
void AManadropPool::BeginPlay()
{
	Super::BeginPlay();

	manaDropBPClass = TSoftClassPtr<AActor>( FSoftObjectPath( TEXT( "Blueprint'/Game/Blueprints/Items/BP_LootableMana.BP_LootableMana'" ) ) ).LoadSynchronous();

	for( int i = 0; i < initialPoolSize; i++ )
	{
		pool.Add( createManaDrop() );
	}
}

void AManadropPool::spawnDrop( FVector location, bool isLightMode, float manaAmount, float expAmount )
{
	for( AManaDrop* mana : pool )
	{
		if( mana->IsHidden() )
		{
			mana->spawn( isLightMode, manaAmount, expAmount );
			mana->SetActorLocation( location );

			return;
		}
	}

	//If theres no available mana
	AManaDrop* mana = createManaDrop();
	pool.Add( mana );

	mana->spawn( isLightMode, manaAmount, expAmount );
	mana->SetActorLocation( location );

}

AManaDrop* AManadropPool::createManaDrop()
{
	FRotator Rot = FRotator::ZeroRotator;
	FVector Loc = GetActorLocation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bNoFail = true;
	SpawnParams.bAllowDuringConstructionScript = false;
	SpawnParams.bDeferConstruction = false;

	return Cast<AManaDrop>(GetWorld()->SpawnActor( manaDropBPClass, &Loc, &Rot, SpawnParams ));
}

