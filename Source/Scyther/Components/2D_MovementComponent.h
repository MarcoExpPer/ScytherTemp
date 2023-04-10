// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "2D_MovementComponent.generated.h"


UENUM( BlueprintType, meta = ( DisplayName = "Player states" ) )
enum class PlayerStates: uint8
{
	WALKING UMETA( DisplayName = "Caminando" ),
	JUMPING UMETA( DisplayName = "Saltando" ),
	RISING  UMETA( DisplayName = "Ascendiendo" ),
	FALLING  UMETA( DisplayName = "Cayendose" ),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCYTHER_API U2D_MovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Walk ", meta = ( DisplayName = "Max X speed"))
		float maxXspeed = 360;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Falling ", meta = ( DisplayName = "Max Fall speed" ) )
		float maxFallSpeed = 1000;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Falling ", meta = ( DisplayName = "Falling Gravity" ) )
		float fallingGravity = 3000;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Jump ", meta = ( DisplayName = "Max Y Speed" ) )
		float maxYspeed = 700;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Jump ", meta = ( DisplayName = "Jump acceleration" ) )
		float jumpAcceleration = 7000;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Jump ", meta = ( DisplayName = "Max jump duration" ) )
		float maxJumpDuration = 0.2333;

	float currentJumpTimer = 0;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Movement Component | Jump ", meta = ( DisplayName = "Rising Gravity" ) )
		float risingGravity = 2100;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Movement Component | General ", meta = ( DisplayName = "Player state" ) )
		PlayerStates state = PlayerStates::WALKING;

	FVector frameVelocity;
	FVector accumVelocity;

	UCapsuleComponent *capsule;

	TMap<PlayerStates, float> stateToGravity;
	TMap<PlayerStates, float> stateToMaxYspeed;

	FTimerHandle groundedHandle;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Sets default values for this component's properties
	U2D_MovementComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION( BlueprintCallable, meta = ( DisplayName = "Add Velocity") )
	void addVelocity( FVector direction, float scale );

	void applyGravity();

	void updateMovement( float DeltaTime );
		
	UFUNCTION( BlueprintCallable, meta = ( DisplayName = "Change player state" ) )
		bool changeState( PlayerStates newState );

	UFUNCTION( BlueprintCallable, meta = ( DisplayName = "Start jump" ) )
		void startJump();

	UFUNCTION( BlueprintCallable, meta = ( DisplayName = "Jump loop" ) )
		void jumping( float DeltaTime );

	UFUNCTION( BlueprintCallable, meta = ( DisplayName = "Jump end" ) )
		void stopJump( bool instantStop = false);

	UFUNCTION()
	void onHitCallback(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit );

	FString GetStateAsName(PlayerStates EnumValue);
	void isGrounded();
};
