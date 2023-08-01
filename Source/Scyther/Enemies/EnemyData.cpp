// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyData.h"
#include "BaseEnemy.h"
#include "BaseEnemyCtrl.h"


// Sets default values
AEnemyData::AEnemyData()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyData::BeginPlay()
{
	Super::BeginPlay();
	if( EnemySpawnPosition == nullptr )
	{
		UE_LOG( LogTemp, Error, TEXT( "EnemySpawnPosition is null!" ) );
		return;
	}
	
	SpawnEnemy( EnemySpawnPosition->GetActorLocation(), EnemySpawnPosition->GetActorRotation() );
}

// Called every frame
void AEnemyData::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AEnemyData::SpawnEnemy(FVector Pos, FRotator Rot)
{
	GetWorld()->SpawnActor<ABaseEnemy>( EnemyClass, Pos, Rot );
	GetWorld()->SpawnActor<ABaseEnemyCtrl>( EnemyController, Pos, Rot );

}

void AEnemyData::UpdatedEnemyType( enum ChooseEnemyType _type )
{
	EnemyType = _type;

	switch( EnemyType )
	{
	case NUBERU:
		//SpawnEnemy( EnemyClass->StaticClass() );
		break;
	case RABBIT:
		//SpawnEnemy( RabbitPawn );
		break;
	case WORM:
		//SpawnEnemy( WormPawn );
		break;
	default:
		break;
	}
}

