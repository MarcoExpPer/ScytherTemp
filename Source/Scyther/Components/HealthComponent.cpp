// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"
#include "SkillComponent.h"

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

	InitInmunities( DamageModes::DASHEABLE );
	InitInmunities( DamageModes::INEVITABLE );
	InitInmunities( DamageModes::DEADZONE );
	InitInmunities( DamageModes::HEALING );
}

void UHealthComponent::InitInmunities(DamageModes mode)
{
	bool* isInmune = inmunities.Find( mode );
	if( isInmune == nullptr )
	{
		inmunities.Add( mode, false );
	}
}


// Called every frame
void UHealthComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

void UHealthComponent::changeHealth( float healthOffset, DamageModes type, bool sendEvent)
{
	float oldHp = currentHp;
	currentHp = FMath::Clamp<float>( ( currentHp + healthOffset ), 0.0f, maxHp );

	if( currentHp == 0 )
	{
		if( saveLastHp && type != DamageModes::DEADZONE && oldHp > 5 )
		{
			currentHp = 1;
			saveLastHp = false;
		}
		else
		{
			zerohealthEvent.Broadcast( type );
		}
	}

	if( sendEvent )
	{
		healthChangedEvent.Broadcast( oldHp, currentHp, type );
	}
}

void UHealthComponent::receiveDamage( float damage, DamageModes type, bool sendEvent)
{
	bool* isInmune = inmunities.Find( type );

	if( isInmune == nullptr )
	{
		inmunities.Add( type, false );
		isInmune = false;
	}

	if( *isInmune)
	{
		inmuneHitEvent.Broadcast( type );
	}
	else
	{
		changeHealth( -damage, type, sendEvent );

		//Si es el jugador, cortamos el combo
		AActor* owner = GetOwner();
		if( owner->GetClass()->IsChildOf( AScytherPlayerPawn::StaticClass() ) )
		{
			AScytherPlayerPawn* character = (AScytherPlayerPawn*) owner;
			UAttackComponent* attackComp = character->attackComponent;
			USkillComponent* skillComp = character->skillComponent;
			if( !skillComp->IsOnLightSkill() )
			{
				switch( attackComp->state )
				{
				case AttackState::NONE:
					break;
				case AttackState::FIRST:
					if( attackComp->firstAttackDamageBreaksCombo )
					{
						attackComp->CancelAttack( true );
					}
					break;
				case AttackState::SECOND:
					if( attackComp->secondAttackDamageBreaksCombo )
					{
						attackComp->CancelAttack( true );
					}
					break;
				case AttackState::THIRD:
					if( attackComp->finalAttackDamageBreaksCombo )
					{
						attackComp->CancelAttack( true );
					}
					break;
				case AttackState::AIR:
					if( attackComp->airAttackDamageBreaksCombo )
					{
						attackComp->CancelAttack( true );
					}
					break;
				default:
					break;
				}
			}

			becomeGod();
			GetWorld()->GetTimerManager().SetTimer( InvencibilityTimerHandle, this, &UHealthComponent::becomeMortal, invencibilityTime );
		}
	}
}


void UHealthComponent::becomeGod()
{
	inmunities.Add( DamageModes::DASHEABLE, true );
	inmunities.Add( DamageModes::INEVITABLE, true );
}

void UHealthComponent::becomeMortal()
{
	inmunities.Add( DamageModes::DASHEABLE, false );
	inmunities.Add( DamageModes::INEVITABLE, false );
}

void UHealthComponent::enableDashInmune()
{
	inmunities.Add( DamageModes::DASHEABLE, true );
}

void UHealthComponent::disableDashInmune()
{
	inmunities.Add( DamageModes::DASHEABLE, false );
}

	
	

