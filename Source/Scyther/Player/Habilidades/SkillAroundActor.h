// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/SphereComponent.h>
#include "SkillAroundActor.generated.h"


UCLASS()
class SCYTHER_API ASkillAroundActor : public AActor
{
	GENERATED_BODY()
public:
//Components
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( DisplayName = "Collision Sphere" ) )
	USphereComponent* collision;

//Variables
protected:
	TArray<AActor*> ActorsAffected;

public:	
	// Sets default values for this actor's properties
	ASkillAroundActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION( BlueprintCallable )
	void onOverlapAtStart();

	UFUNCTION()
	void onOverlapEnter( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
	UFUNCTION()
	void onOverlapExit( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );

	UFUNCTION( BlueprintCallable )
	void endSkill();

	virtual void onStartOverlapActor(AActor * actor);
	virtual void onOverlapEnterActor( AActor* actor );
	virtual void onOverlapExitActor( AActor* actor );
	virtual void onEndSkillActor( AActor* actor );

	
};
