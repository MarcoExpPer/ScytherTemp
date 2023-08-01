// Fill out your copyright notice in the Description page of Project Settings.


#include "LootableHealth.h"
#include <Scyther/Player/ScytherPlayerPawn.h>
#include <Scyther/Components/HealthComponent.h>

void ALootableHealth::playerMeshOverlap( AScytherPlayerPawn* player )
{
	if( !doUIAnimation )
	{
		player->healthComponent->changeHealth( healthAmount, DamageModes::HEALING );
	}
}

void ALootableHealth::setHealthParameters( float newAmount )
{
	healthAmount = newAmount;
	activateLootable();
}