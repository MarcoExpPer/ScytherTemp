// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameViewportClient.h"
#include "Engine/Engine.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BoxSideScrollCamera.generated.h"




UCLASS()
class SCYTHER_API ABoxSideScrollCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoxSideScrollCamera();


	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	class UBoxComponent* main;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UCameraComponent* mainCamera;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UBoxComponent* outsideBox;
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UBoxComponent* insideBox;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Camera Helps", meta = ( DisplayName = "Left limit" ) )
		int leftLimit;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Camera Helps", meta = ( DisplayName = "Right limit" ) )
		int rightLimit;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Camera Helps", meta = ( DisplayName = "Debug Positions" ) )
		bool debugPositions = false ;
	
	APlayerController* player;
	FVector playerPosition;


	UGameViewportClient* viewportClient;
	FIntPoint screenResolution;
	float screenWidth;
	float screenHeight;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
