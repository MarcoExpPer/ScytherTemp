// Fill out your copyright notice in the Description page of Project Settings.


#include "DashComponent.h"
#include "./3D_MovementComponent.h"
#include "./HealthComponent.h"
#include "./AttackComponent.h"
#include <DrawDebugHelpers.h>
#include "Kismet/GameplayStatics.h"
#include "../Player/ScytherPlayerPawn.h"

UDashComponent::UDashComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UDashComponent::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<AScytherPlayerPawn>( GetOwner() );
	control = Cast<AScytherPlayerController>( UGameplayStatics::GetPlayerController( GetWorld(), 0 ) );
}


// Called every frame
void UDashComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if( !hasDashFinished && player->state != MovementState::DASHING )
	{
		DashFinishTimerHandle.Invalidate();
		OnDashFinish();

		DashInmunityTimerHandle.Invalidate();
		OnInmunityDashFinish();
	}

	if( dashTimer > 0 )
	{
		dashTimer -= DeltaTime;
	}

	if( dashTimer <= 0 && isDashAvailable == false )
	{
		PlayDashAvailableVFX();
		isDashAvailable = true;
	}

#if WITH_EDITOR 
	if( activateTextDebug )
	{
		bool const canDash = hasDashFinished && ( dashTimer <= 0 || player->isGodMode );

		GEngine->AddOnScreenDebugMessage( -1, 0, canDash ? FColor::Green : FColor::Red, FString::Printf( TEXT( "Can use dash?: %d" ), canDash ) );
		GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, FString::Printf( TEXT( "Dash Cooldown %.2f - Dash Timer %.2f " ), dashCooldown, dashTimer ) );
	}
#endif

#if WITH_EDITOR 
	if( activateTextDebug )
	{
		GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Blue, FString::Printf( TEXT( "--- DASH ---" ) ) );
	}
#endif
}

void UDashComponent::DashPressed()
{
	bool const canDash = hasDashFinished && ( dashTimer <= 0 || player->isGodMode ) && player->state != MovementState::KNOCKBACK;

	if( canDash && player->state == MovementState::ATTACKING && player->attackComponent->state != AttackState::AIR )
	{
		SaveDashInput();

		return;
	}

	DashStartEvent.Broadcast( canDash );

	if( canDash )
	{
		StartDash();
	}
	else
	{
		//Sounds of the dash when it is not available
		if( DashNotAvailable )
		{
			UFMODBlueprintStatics::PlayEvent2D( GetWorld(), DashNotAvailable, true );
		}
	}
}


void UDashComponent::SaveDashInput()
{
	isDashSaved = true;
}

bool UDashComponent::CheckDashInputSaved()
{
	if( isDashSaved )
	{
		isDashSaved = false;

		bool const canDash = hasDashFinished && ( dashTimer <= 0 || player->isGodMode ) && player->state != MovementState::KNOCKBACK;

		if( canDash )
		{
			StartDash();
		}
		else
		{
			//Sounds of the dash when it is not available
			if( DashNotAvailable )
			{
				UFMODBlueprintStatics::PlayEvent2D( GetWorld(), DashNotAvailable, true );
			}
		}

		return true;
	}
	return false;
}

void UDashComponent::StartDash()
{
	player->healthComponent->enableDashInmune();

	player->state = MovementState::DASHING;
	hasDashFinished = false;
	dashTimer = dashCooldown;

	savedVelocity = player->movementComp->velocity;
	savedVelocity.Z = savedVelocity.Z > 0 ? 0 : savedVelocity.Z;

	FVector currentDirection = FVector( control->inputRight, -control->inputDown, 0 );
	currentDirection.Z = 0;
	currentDirection.Normalize();

	if( currentDirection.IsNearlyZero() )
	{
		currentDirection = GetOwner()->GetActorForwardVector();
		currentDirection = idleDashForward ? currentDirection : currentDirection * -1;

		player->movementComp->velocity = currentDirection * ( ( dashDistance * ( dashDistanceMultiplicator / 100 ) * ( idleDashDistanceMultiplier / 100 ) ) / dashDuration );
	}
	else
	{
		player->movementComp->velocity = currentDirection * ( ( dashDistance * dashDistanceMultiplicator / 100 ) / dashDuration );
	}

	player->movementComp->velocity.Z = dashVerticalSpeed;

	GetOwner()->GetWorldTimerManager().SetTimer(
		DashFinishTimerHandle, this, &UDashComponent::OnDashFinish, dashDuration, false );

	GetOwner()->GetWorldTimerManager().SetTimer(
		DashInmunityTimerHandle, this, &UDashComponent::OnInmunityDashFinish, dashInmunityDuration, false );

	StartDashVFXAndSFX();
}

void UDashComponent::OnDashFinish()
{
	hasDashFinished = true;
	isDashAvailable = false;
	isDashSaved = false;

	DashEndEvent.Broadcast();

	if( player->movementComp->isMovementActive )
	{
		player->movementComp->velocity = savedVelocity;
		player->ChangeMovementState(MovementState::FALLING, true);
	}
}

void UDashComponent::OnInmunityDashFinish()
{
	if( !player->isGodMode )
	{
		player->healthComponent->disableDashInmune();
	}
}

void UDashComponent::StartDashVFXAndSFX()
{
	//Sounds of the dash
	if( Dash )
	{
		UFMODBlueprintStatics::PlayEvent2D( GetWorld(), Dash, true );
	}

	player->dashNiagaraSystem->Activate();
	GetOwner()->GetWorldTimerManager().SetTimer(
			DashVFXTimerHandle, this, &UDashComponent::OnVFXDashFinish, dashDuration, false );
}

void UDashComponent::OnVFXDashFinish()
{
	player->dashNiagaraSystem->Deactivate();
}

void UDashComponent::PlayDashAvailableVFX()
{
	player->dashAvailableNiagaraSystem->Activate();
}