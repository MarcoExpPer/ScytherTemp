// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "fmod.hpp"
#include "FMODEvent.h"
#include "FMODBlueprintStatics.h"
#include <FMODAudioComponent.h>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "BreakableFloor.generated.h"


DECLARE_DELEGATE( FDestroyPlatformEventSignature )
UCLASS()
class SCYTHER_API ABreakableFloor: public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABreakableFloor();

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Design | Platform", meta = ( DisplayName = "Distance that the platform fall",
		ToolTip = "Distancia que cae la plataforma en la animacion. Medida en unidades de unreal (cm). \nNOTA: La plataforma siempre va a caer, por lo que, el valor debe ser positivo." ) )
		FVector FallDistance;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Design | Platform", meta = ( DisplayName = "Platform Duration Before Rotation",
		ToolTip = "Tiempo que esta la plataforma esta activa y que tarda en rotar despues de que el jugador la haya pisado" ) )
		float PlatformDurationRotation = 0.35f;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Design | Platform", meta = ( DisplayName = "Platform Rotation",
		ToolTip = "Rotacion de la plataforma (en grados)" ) )
		FRotator PlatformRotation;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Design | Platform", meta = ( DisplayName = "Platform Duration After Rotation And Before Destruction",
		ToolTip = "Tiempo que tarda la plataforma en romperse despues de rotar" ) )
		float PlatformDurationDestruction = 0.85f;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Design | Platform", meta = ( DisplayName = "Platform Duration",
		ToolTip = "Tiempo que esta la plataforma activa" ) )
		float PlatformDuration = 2.0f;*/

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Design | Platform", meta = ( DisplayName = "Respawn Platform",
		ToolTip = "Quieres que respawnee?" ) )
		bool Respawn = true;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Design | Platform", meta = ( DisplayName = "Time Platform Respawn",
		ToolTip = "Tiempo que tarda la plataforma en respawnear" ) )
		float PlatformRespawn = 5.0f;
 
	//FMOD

	FFMODEventInstance InstanceWraper;

	UPROPERTY(EditAnywhere, Category = "Audio | FMOD", meta = ( DisplayName = "Audio Cracking Wood",
		ToolTip = "Cruje la madera/plataforma cuando el jugador la pisa" ) )
		UFMODEvent* EventCrackingWood;

	UPROPERTY( EditAnywhere, Category = "Audio | FMOD", meta = ( DisplayName = "Audio Breaking Wood",
		ToolTip = "Audio de cuando se rompe la madera/plataforma" ) )
		UFMODEvent* EventBreakingWood;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Componentes" )
		UStaticMeshComponent* BreakableFloor;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		UBoxComponent* BreakableFloorCollision;

private:

	FTimerHandle PlatformDurationRotationTimerHandle;
	FTimerHandle PlatformDestructionDurationTimerHandle;
	FTimerHandle PlatformRespawnTimerHandle;
	bool isDespawning = false;
	FName PlayerBody;
	FTransform originalPlatform;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

	UFUNCTION()
		void BeginCollision( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	void RotatePlatform();

	void HidePlatform();


	//UFUNCTION()
	//	void PlayerDeath( ElementTypes type );

	void RespawnPlatform();


};