// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyData.generated.h"

UENUM()
enum ChooseEnemyType
{
	RABBIT UMETA( DisplayName = "Conejo" ),
	WORM UMETA( DisplayName = "Gusano" ),
	NUBERU UMETA( DisplayName = "Nuberu" ),
};

UCLASS()
class SCYTHER_API AEnemyData : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyData();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY( EditAnywhere, Category = "Enemy", meta = ( DisplayName = "EnemyType" ) )
		TEnumAsByte<ChooseEnemyType> EnemyType;
	UPROPERTY( EditAnywhere, Category = "Enemy", meta = ( DisplayName = "EnemyPosition" ) )
		AActor* EnemySpawnPosition;
	UPROPERTY( EditAnywhere, Category = "Enemy", meta = ( DisplayName = "SelectEnemy" ) )
	TSubclassOf<class ABaseEnemy> EnemyClass;
	UPROPERTY( EditAnywhere, Category = "Enemy", meta = ( DisplayName = "SelectEnemy" ) )
	TSubclassOf<class ABaseEnemyCtrl> EnemyController;

	void UpdatedEnemyType(ChooseEnemyType _type );
	void SpawnEnemy(FVector Pos, FRotator Rot);

};
