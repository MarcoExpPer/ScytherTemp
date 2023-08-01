// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "fmod.hpp"
#include "FMODEvent.h"
#include "FMODBlueprintStatics.h"
#include <FMODAudioComponent.h>
#include "DestructibleComponent.h"

#include "ItemDestructible.generated.h"
UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FDoHitSound );
UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FDestroySound );
UCLASS()
class SCYTHER_API AItemDestructible: public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemDestructible();
	UFUNCTION()
		void DisabledGravity( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
	UFUNCTION()
		void DisabledGravityHit( UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );
	UFUNCTION()
		void EnabledGravity( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );
	UFUNCTION()
		void Trigger( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
	UFUNCTION()
		void Destroy( DamageModes damageType );
	UFUNCTION()
		void DeSpawn();

	UPROPERTY( BlueprintAssignable )
		FDoHitSound DoHitSound;

	UPROPERTY( BlueprintAssignable )
		FDestroySound DestroySound;

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Destructible" )
		class UBoxComponent* TriggerComponent;

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Destructible" )
		class UDestructibleComponent* DestructibleComponent;

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Destructible" )
		class UBoxComponent* DetectGravity;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Destructible" )
		bool IsDestroyed = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Components" )
		class UHealthComponent* healthComp;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Components" )
		class UDropLooteablesComponent* LooteableComp;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Components" )
		class UFMODAudioComponent* FMODAudio;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Destructible" )
		float DefaultDamage = 1.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Destructible" )
		float DefaultImpulse = 1.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Destructible" )
		int HitsToDestroy = 1;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Destructible" )
		float ImpulseStrength = 1.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Destructible" )
		bool HasPhysics = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Destructible" )
		bool CanDestroy = true;

	//FFMODEventInstance InstanceWraper;



	int hitsReceived = 0;
	FVector HitLocation;
	FVector ImpulseDir;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

};
