// Fill out your copyright notice in the Description page of Project Settings.


#include "DropLooteablesComponent.h"
#include <Scyther/ScytherGameModeBase.h>
#include <Scyther/Items/LootableItem.h>
#include <Scyther/Items/LootableHealth.h>
#include <Scyther/Items/LootableMana.h>
#include <Kismet/GameplayStatics.h>

// Sets default values for this component's properties
UDropLooteablesComponent::UDropLooteablesComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDropLooteablesComponent::BeginPlay()
{
	Super::BeginPlay();

	gm = Cast<AScytherGameModeBase>( UGameplayStatics::GetGameMode( GetWorld() ) );
}


// Called every frame
void UDropLooteablesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDropLooteablesComponent::DropLooteables( FVector location, bool isLightMana )
{
	if( manaCrystalsAmount > 0 )
	{
		DropManaCrystals( location, gm->manaLooteableBPClass, isLightMana );
	}

	if( healthCrystalAmount > 0 )
	{
		DropHealthCrystals( location, gm->healthLooteableBPClass );
	}


}

void UDropLooteablesComponent::DropManaCrystals( FVector location, TSubclassOf<class ALootableItem> classToSpawn, bool isLightMana )
{
	float manaAmount = isLightMana ? lightAmount / manaCrystalsAmount : darkAmount / manaCrystalsAmount;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FTransform SpawnTransform;

	for( int i = 0; i < manaCrystalsAmount; ++i )
	{
		FVector loc = location;

		loc.X = loc.X + FMath::FRandRange( -100, 100 );
		loc.Y = loc.Y + FMath::FRandRange( -100, 100 );
		loc.Z = loc.Z + FMath::FRandRange( -20, 20 );

		SpawnTransform.SetLocation( loc );

		AActor* a = GetWorld()->SpawnActor( classToSpawn, &SpawnTransform, SpawnParams );
		if( a == nullptr )
		{
			i--;
		}
		else
		{
			ALootableMana* manaCrystal = Cast< ALootableMana>( a );
			
			manaCrystal->SetIsFromWorld( false );
			manaCrystal->doUIAnimation = doUIAnimation;
			manaCrystal->setManaParameters( isLightMana, manaAmount );
		}
		
	}
}

void UDropLooteablesComponent::DropHealthCrystals( FVector location, TSubclassOf<class ALootableItem> classToSpawn )
{
	float realHealthAmount = healthAmount / healthCrystalAmount;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FTransform SpawnTransform;

	for( int i = 0; i < healthCrystalAmount; ++i )
	{
		FVector loc = location;

		loc.X = loc.X + FMath::FRandRange( -100, 100 );
		loc.Y = loc.Y + FMath::FRandRange( -100, 100 );
		loc.Z = loc.Z + FMath::FRandRange( -20, 20 );

		SpawnTransform.SetLocation( loc );

		AActor* a = GetWorld()->SpawnActor( classToSpawn, &SpawnTransform, SpawnParams );
		if( a == nullptr )
		{
			i--;
		}
		else
		{
			ALootableHealth* healthCrystal = Cast< ALootableHealth>( a );
			healthCrystal->SetIsFromWorld( false );
			healthCrystal->doUIAnimation = doUIAnimation;
			healthCrystal->setHealthParameters( realHealthAmount );
		}
		
	}
}

