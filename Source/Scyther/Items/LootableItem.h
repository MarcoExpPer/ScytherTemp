// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "fmod.hpp"
#include "FMODEvent.h"
#include "FMODBlueprintStatics.h"
#include <FMODAudioComponent.h>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Scyther/Interfaces/CanRespawn.h>
#include "LootableItem.generated.h"

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FLooted );

UCLASS()
class SCYTHER_API ALootableItem: public AActor, public ICanRespawn
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALootableItem();

	UPROPERTY( BlueprintAssignable )
		FLooted LootedEvent;

	//-- COMPONENTS --//
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Mana", meta = ( AllowPrivateAccess = "true" ) )
		class UStaticMeshComponent* mesh;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class USphereComponent* sphere;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class URotatingMovementComponent* rotComp;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UStaticMeshComponent* entityShadow;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>Player will Traverse on giving spawn point in the world.
		class UEntityShadow* entityShadowComponent;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		/// <summary><value>Player will Traverse on giving spawn point in the world.
		class ARespawnManager* respawnMan;

	//-- PARAMETERS --//
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Lootable", meta = ( Units = "s" ) )
		float timeToGetToPlayer = 1.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Lootable" )
		FRotator rotationRate = FRotator( 10.f, 20.f, 30.f );

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Lootable" )
		bool isReady = false;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Lootable" )
		bool isFromWorld = true;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Lootable" )
		bool doUIAnimation = false;

	//-- FMOD --//
	UPROPERTY( EditAnywhere, Category = "Audio | FMOD", meta = ( DisplayName = "Lootable Light Mana Sound",
		ToolTip = "Pick Up Lootable Light Mana" ) )
		UFMODEvent* LootableSound;


private:
	bool isFlyingTowardsPlayer = false;
	float currentTimer = 0;
	

	FTransform initialTransform;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

	void SetIsFromWorld( bool newIsFromWorld );

	UFUNCTION( BlueprintCallable )
		void activateLootable();

	UFUNCTION()
		void OnMeshOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
	UFUNCTION()
		void OnSphereOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	UFUNCTION()
		void OnPlayerRespawn();

	virtual void Respawn_Implementation() override;

protected:
	virtual void playerMeshOverlap( class AScytherPlayerPawn* player );

private:
	void ComponentOverlapWithMesh( UPrimitiveComponent* comp );
	void ComponentOverlapWithSphere( UPrimitiveComponent* comp );
};
