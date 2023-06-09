// Fill out your copyright notice in the Description page of Project Settings.


#include "RabbitCtrl.h"

ARabbitCtrl::ARabbitCtrl()
{

}

void ARabbitCtrl::BeginPlay()
{
	Super::BeginPlay();

	rabbitPawn = Cast<ARabbitPawn>( this->GetPawn() );

	distanceToDetectPlayer = rabbitPawn->distanceToDetectPlayer;
	distanceToAttackPlayer = rabbitPawn->distanceToAttackPlayer;
}


void ARabbitCtrl::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}



void ARabbitCtrl::moveToPosition( FVector targetPosition )
{
	if( rabbitPawn != nullptr )
	{
		rotatePawnTowardsTargetXY( targetPosition );
		this->MoveToLocation( targetPosition, 1.0f, false, true );
	}
}

bool ARabbitCtrl::DoBasicAttack( FVector targetPosition )
{
	if( rabbitPawn != nullptr )
	{
		rotatePawnTowardsTargetXY( targetPosition );
		this->StopMovement();
		rabbitPawn->Attack();
		return true;
	}

	return false;
}