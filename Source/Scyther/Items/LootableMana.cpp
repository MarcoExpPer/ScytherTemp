// Fill out your copyright notice in the Description page of Project Settings.


#include "LootableMana.h"
#include <Scyther/Player/ScytherPlayerPawn.h>
#include <Scyther/Components/SkillComponent.h>

void ALootableMana::playerMeshOverlap( AScytherPlayerPawn* player )
{
	if( !doUIAnimation )
	{
		player->skillComponent->UpdateMana( true, manaAmount );
	}
}

void ALootableMana::setManaParameters( bool newIsLightMana, float newAmount  )
{
	isLightMana = true;
	manaAmount = newAmount;

	if( IsLightManaChangedEvent.IsBound() )
	{
		IsLightManaChangedEvent.Broadcast();
	}

	activateLootable();
}