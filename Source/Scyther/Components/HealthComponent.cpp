// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	currentHp = FMath::Clamp<float>( ( initialHp ), 0.0f, maxHp );
}


// Called every frame
void UHealthComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

void UHealthComponent::changeHealth( float healthOffset, ElementTypes type, bool sendEvent)
{
	float oldHp = currentHp;
	currentHp = FMath::Clamp<float>( ( currentHp + healthOffset ), 0.0f, maxHp );

	if( sendEvent )
	{
		healthChangedEvent.Broadcast( oldHp, currentHp, type );
	}

	if( currentHp == 0 )
	{
		zerohealthEvent.Broadcast( type );
	}

#if WITH_EDITOR 
	UE_LOG( LogTemp, Warning, TEXT( "Entidad recibe %f cambio de vida. Ahora tiene %f vida" ), healthOffset, currentHp );
#endif

}

void UHealthComponent::receiveDamage( float damage, ElementTypes type, bool sendEvent)
{
	if( isGodMode )
	{
		return;
	}

	float* vulnerabilityFactor = vulnerabilities.Find( type );

	if( vulnerabilityFactor == nullptr )
	{
		changeHealth( -damage, ElementTypes::NEUTRAL, sendEvent );
	}
	else
	{
		float realDamage = damage * ( *vulnerabilityFactor );
		changeHealth( -realDamage, type, sendEvent );
	}


}
	
	

