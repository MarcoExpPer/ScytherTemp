// Fill out your copyright notice in the Description page of Project Settings.


#include "BloodComponent.h"

// Sets default values for this component's properties
UBloodComponent::UBloodComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UBloodComponent::BeginPlay()
{
	Super::BeginPlay();

	currentBlood = initialBlood;
}


// Called every frame
void UBloodComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}



void UBloodComponent::becomeGod()
{
	isInGodMode = true;
	changeBlood( maxBlood );
}

void UBloodComponent::becomeMortal()
{
	isInGodMode = false;
}



void UBloodComponent::changeBlood( float change, bool sendEvent )
{
	float oldBlood = currentBlood;
	float newBlood = currentBlood + change;
	if( newBlood > maxBlood || isInGodMode )
	{
		newBlood = maxBlood;
	}
	else if( newBlood < 0 )
	{
		newBlood = 0;
	}
	currentBlood = newBlood;
	if( oldBlood!= newBlood && sendEvent)
	{
		bloodChangedEvent.Broadcast( oldBlood, newBlood );
	}
}

float UBloodComponent::useBlood( float healthMissing, bool sendEvent )
{
	float bloodToUse = currentBlood < healthMissing ? currentBlood : healthMissing;

	if( !isInGodMode )
	{
		changeBlood( -bloodToUse );
	}
	if( sendEvent )
	{
		bloodUsedEvent.Broadcast( currentBlood );
	}
	return bloodToUse;
}

