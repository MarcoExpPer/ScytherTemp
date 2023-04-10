// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/SphereComponent.h>

#include "ManaDrop.generated.h"

UCLASS()
class SCYTHER_API AManaDrop : public AActor
{
	GENERATED_BODY()
	
public:	

	//-- COMPONENTS --//
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Mana", meta = ( AllowPrivateAccess = "true" ) )
		UStaticMeshComponent* mesh;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		USphereComponent* sphere;

	//-- MATERIALS --/
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Mana", meta = ( AllowPrivateAccess = "true" ) )
		UMaterial* lightMat;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Mana", meta = ( AllowPrivateAccess = "true" ) )
		UMaterial* darkMat;

	//-- MOVEMENT PARAMETERS --/

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		float timeToGetToPlayer = 2;

	float currentTimer = 0;

private:
	bool isFlyingToPlayer = false;

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Mana", meta = (
		ToolTip = "Si esta a true, el mana esta en modo luz" ) )
	bool isLightLoot = true;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Mana", meta = (
		ToolTip = "Cantidad de experiencia que suma al jugador" ) )
	float expAmount = 10;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Mana", meta = (
		ToolTip = "Cantidad de mana que suma al jugador" ) )
	float manaAmount = 10;

	AManaDrop();

	UFUNCTION()
	void OnMeshOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	UFUNCTION()
	void OnSphereOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	UFUNCTION( BlueprintCallable )
		void deSpawn();
	UFUNCTION( BlueprintCallable )
		void spawn( bool newMode, float newMana, float newExp );

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void StartFlying();
};
