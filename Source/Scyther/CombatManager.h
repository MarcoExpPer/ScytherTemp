// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatManager.generated.h"

class ABaseEnemyCtrl;

UCLASS()
class SCYTHER_API ACombatManager : public AActor
{
	GENERATED_BODY()


	class AScytherGameModeBase* gm;

private:
	int inCombatMaxPoolSize = 4; 
	int currentCombatPoolSize = 0;

	TArray<ABaseEnemyCtrl*> inCombatEnemies;
	TArray<ABaseEnemyCtrl*> idleEnemies;
	TArray<ABaseEnemyCtrl*> goingToIdleEnemies;

public:	
	// Sets default values for this actor's properties
	ACombatManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void addEnemyToIdleList( class ABaseEnemy* enemyToAdd, bool addToGoingToIdle = false );

	void refreshInCombatList();

	void SortIdleByDistance();

	void MaxNumberOfAttacksCompleted( ABaseEnemyCtrl* ctrlToIdle );

	void RemoveFromAllLists( ABaseEnemyCtrl* ctrlToRemove );
};
