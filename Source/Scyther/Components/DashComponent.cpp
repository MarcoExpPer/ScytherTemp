// Fill out your copyright notice in the Description page of Project Settings.


#include "DashComponent.h"
#include "./3D_MovementComponent.h"
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
}


// Called every frame
void UDashComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if( !hasDashFinished && player->state != MovementState::DASHING )
	{
		DashFinishTimerHandle.Invalidate();
		OnDashFinish();
	}

	if( dashTimer > 0 )
	{
		dashTimer -= DeltaTime;
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

void UDashComponent::StartDash()
{
	bool const canDash = hasDashFinished && ( dashTimer <= 0 || player->isGodMode );
	DashStartEvent.Broadcast( canDash );

	if( canDash )
	{
		player->state = MovementState::DASHING;
		hasDashFinished = false;
		dashTimer = dashCooldown;

		savedVelocity = player->movementComp->velocity;
		savedVelocity.Z = savedVelocity.Z > 0 ? 0 : savedVelocity.Z;

		FVector currentDirection = savedVelocity;
		currentDirection.Z = 0;
		currentDirection.Normalize();

		if( currentDirection.IsNearlyZero() )
		{
			currentDirection = GetOwner()->GetActorForwardVector();
		}

		player->movementComp->velocity = currentDirection * ( dashDistance / dashDuration );

		GetOwner()->GetWorldTimerManager().SetTimer(
			DashFinishTimerHandle, this, &UDashComponent::OnDashFinish, dashDuration, false );
	}
}

void UDashComponent::OnDashFinish()
{
	hasDashFinished = true;

	DashEndEvent.Broadcast();

	player->movementComp->velocity = savedVelocity;
	player->state = MovementState::FALLING;
}
