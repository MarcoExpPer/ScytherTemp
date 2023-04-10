// Fill out your copyright notice in the Description page of Project Settings.


#include "2D_MovementComponent.h"

// Sets default values for this component's properties
U2D_MovementComponent::U2D_MovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	stateToGravity.Add(PlayerStates::RISING, risingGravity);
	stateToGravity.Add(PlayerStates::JUMPING, 0);


	stateToMaxYspeed.Add(PlayerStates::RISING, 0);
	stateToMaxYspeed.Add(PlayerStates::JUMPING, 0);
}


// Called when the game starts
void U2D_MovementComponent::BeginPlay()
{
	Super::BeginPlay();
	capsule = GetOwner()->FindComponentByClass<UCapsuleComponent>();

	check(capsule != nullptr);

	capsule->OnComponentHit.AddDynamic(this, &U2D_MovementComponent::onHitCallback);
}


// Called every frame
void U2D_MovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	applyGravity();
	jumping(DeltaTime);

#if WITH_EDITOR 
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("VVelocity x: %f, y: %f, z: %f"), accumVelocity.X, accumVelocity.Y, accumVelocity.Z));
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("Frame Velocity x: %f, y: %f, z: %f"), frameVelocity.X, frameVelocity.Y, frameVelocity.Z));
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, GetStateAsName(state));
#endif

	updateMovement(DeltaTime);


}


void U2D_MovementComponent::addVelocity(FVector direction, float scale)
{

	frameVelocity = ((GetOwner()->GetWorld()->DeltaTimeSeconds * scale) * direction) + frameVelocity;

}

void U2D_MovementComponent::updateMovement(float DeltaTime)
{
	FVector realFrameVelocity = FVector::FVector(0.f, 0.f, 0.f);

	if (frameVelocity.X * accumVelocity.X <= 0.f)
	{
		realFrameVelocity.X = frameVelocity.X;
	}
	else
	{
		realFrameVelocity.X = frameVelocity.X + accumVelocity.X;
	}

	float* maxZspeed = stateToMaxYspeed.Find(state);
	if (maxZspeed == nullptr)
	{
		realFrameVelocity.Z = FMath::Clamp(frameVelocity.Z + accumVelocity.Z, -maxFallSpeed, maxYspeed) *DeltaTime;
	}
	else
	{
		realFrameVelocity.Z = FMath::Clamp(frameVelocity.Z + accumVelocity.Z, -*maxZspeed, maxYspeed) * DeltaTime;
	}

	realFrameVelocity.X = FMath::Clamp(realFrameVelocity.X, -maxXspeed, maxXspeed) * DeltaTime;

	FVector oldLocation = GetOwner()->GetActorLocation();

	FHitResult hit;
	GetOwner()->SetActorLocation(oldLocation + realFrameVelocity, true, &hit);

	while (hit.bBlockingHit)
	{
		FVector traceVector = hit.TraceEnd - hit.TraceStart;

		float scalarProduct = FVector::DotProduct(hit.ImpactNormal, traceVector);

		FVector normalXscalar = hit.ImpactNormal * scalarProduct;

		FVector newMovement = traceVector - normalXscalar;
		FVector newOldLocation = GetOwner()->GetActorLocation();

		GetOwner()->SetActorLocation(newOldLocation + newMovement, true, &hit);
	}


	accumVelocity = (GetOwner()->GetActorLocation() - oldLocation) / DeltaTime;
	frameVelocity.Set(0.f, 0.f, 0.f);


	if (state == PlayerStates::JUMPING || state == PlayerStates::RISING)
	{
		if (accumVelocity.Z <= 0.f)
		{
			changeState(PlayerStates::FALLING);
		}
	}

	//FVector oldLocation = GetOwner()->GetActorLocation();

	////Limitar la velocidad horizontal 
	//FVector bert = ( frameVelocity + accumVelocity );
	//bert.X = FMath::Clamp( bert.X, -maxXspeed, maxXspeed );

	////Calcular la nueva posicion
	//FVector newLocation = ( bert * DeltaTime ) + oldLocation;

	//FHitResult hit;
	//GetOwner()->SetActorLocation( newLocation, true, &hit );

	///*Si el movimiento choca con algo, recolocamos al jugador hasta que deje de chocar.
	//En caso de que fuera una pared, el primer ciclo nos empujaria hacia detras impidiendo seguir en el,
	//En caso de chocarse contra una rampa, el producto escalar empujaria al jugador hacia arriba de la rampa, haciendole
	//subir la rampa "suavemente"
	//*/
	//while( hit.bBlockingHit )
	//{
	//	//Calcular el producto escalar para saber si mover al jugador hacia arriba o hacia abajo
	//	float scalarProduct = FVector::DotProduct( hit.ImpactNormal, ( hit.TraceEnd - hit.TraceStart ) );
	//	//Calcular el movimiento necesario para evitar el choque
	//	FVector movementOffset = ( hit.TraceEnd - hit.TraceStart ) - ( scalarProduct * hit.ImpactNormal );

	//	newLocation = movementOffset + GetOwner()->GetActorLocation();

	//	GetOwner()->SetActorLocation( newLocation, true, &hit );
	//}

	////Si no dividimos por delta time, la velocidad variaría en funcion del periodo de ejecucion de esta funcion, tickrate
	//accumVelocity = (GetOwner()->GetActorLocation() - oldLocation) / DeltaTime;

	//frameVelocity.Set( 0.f, 0.f, 0.f );
}

void U2D_MovementComponent::applyGravity()
{
	float* currentGravity = stateToGravity.Find(state);

	if (currentGravity == nullptr)
	{
		addVelocity(FVector::DownVector, fallingGravity);
	}
	else
	{
		addVelocity(FVector::DownVector, *currentGravity);
	}
}

bool U2D_MovementComponent::changeState(PlayerStates newState)
{
	switch (newState)
	{
	case PlayerStates::WALKING:
	{
		state = newState;

		break;
	}
	case PlayerStates::JUMPING:
	{
		state = newState;

		break;
	}
	case PlayerStates::RISING:
	{
		state = newState;

		break;
	}
	case PlayerStates::FALLING:
	{
		if (state != PlayerStates::JUMPING)
		{
			state = newState;
		}
		else
		{
			return false;
		}
		break;
	}
	}
	return true;
}

void U2D_MovementComponent::startJump()
{
	if (state == PlayerStates::WALKING)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Salto iniciado")));

		changeState(PlayerStates::JUMPING);
		currentJumpTimer = 0;
	}
}

void U2D_MovementComponent::jumping(float DeltaTime)
{
	if (state == PlayerStates::JUMPING)
	{
		addVelocity(FVector::UpVector, jumpAcceleration);

		currentJumpTimer += DeltaTime;

		if (currentJumpTimer >= maxJumpDuration)
		{
			stopJump();
		}
	}
}

void U2D_MovementComponent::stopJump(bool instantStop)
{
	if (state == PlayerStates::JUMPING)
	{
		changeState(PlayerStates::RISING);

		if (instantStop)
		{
			accumVelocity.Z = (accumVelocity.Z / GetOwner()->GetWorld()->DeltaTimeSeconds) / 1.5;
			addVelocity(FVector::DownVector, FMath::Abs(accumVelocity.Z));
		}
	}

}

void U2D_MovementComponent::onHitCallback(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	float a = FVector::DotProduct(Hit.Normal, FVector(0.0, 0.0, 1.0));
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("aaAaAAAaA: %f"), a));
	if (a >= 0.71)
	{
		changeState(PlayerStates::WALKING);
	}

	if (groundedHandle.IsValid()) {
		GetOwner()->GetWorld()->GetTimerManager().ClearTimer(groundedHandle);
	}
	
	GetOwner()->GetWorldTimerManager().SetTimer(
		groundedHandle, this, &U2D_MovementComponent::isGrounded, 0.1f, false);

	//GEngine->AddOnScreenDebugMessage( -1, 5, FColor::Red, FString::Printf( TEXT( "<-------------------->" ) ) );
}

void U2D_MovementComponent::isGrounded() {
	switch (state) {
	case PlayerStates::JUMPING:
	case PlayerStates::RISING:
	case PlayerStates::FALLING:
		return;
	default:
		changeState(PlayerStates::FALLING);
	}
}

FString U2D_MovementComponent::GetStateAsName(PlayerStates EnumValue) {
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("PlayerStates"), true);
	if (!EnumPtr) return FString("Invalid");
	return EnumPtr->GetNameByValue((int64)EnumValue).ToString();
}
