// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../ScytherPlayerPawn.h"
#include "../../Enemies/BaseEnemy.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "LightSkillAttack.generated.h"

UCLASS()
class SCYTHER_API ALightSkillAttack : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY( BlueprintReadOnly )
		UBoxComponent* boxCollision;

	UPROPERTY( EditAnywhere, blueprintReadWrite )
		class UNiagaraComponent* niagaraSystem;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		AScytherPlayerPawn* character;


private:

	float effectDuration;
	float actualEffectDuration;
	float effectDamage;
	float effectWidth;
	float effectHeight;
	FVector effectDirection;
	float effectSpeed;
	UNiagaraSystem* skillVFX;

	TArray<FString> damagedEnemies;

public:	
	// Sets default values for this actor's properties
	ALightSkillAttack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void BeginOverlap( UPrimitiveComponent* OverlappedComponent,
					  AActor* OtherActor,
					  UPrimitiveComponent* OtherComp,
					  int32 OtherBodyIndex,
					  bool bFromSweep,
					  const FHitResult& SweepResult );

	void InitLightSkillAttackValues( FVector newDir, float newDuration, float newDamage, float newWidth, float newHeight, float newSpeed, UNiagaraSystem* newVFX, bool isSpecial );

	void StartVFX();
	void GetImpactPointHitToVFX( UPrimitiveComponent* OtherActor, UPrimitiveComponent* OtherComp );

	bool isADamageableEnemyComp( AActor* actor, UPrimitiveComponent* componentHit );
	bool isADamageableDestructible( AActor* actor );
};
