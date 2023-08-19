/*
Autor: Marco Expósito Pérez.

Last Update: 2/05/2023

File Overview:
Esta clase controla una ronda que es parte de una zona de combate para el jugador.
Idealmente, esta clase heredaria de UOBject y estaria serializada para editar facilmente desde el editor. Debido al tiempo que queda del proyecto y a la cantidad de trabajo que queda por
hacer, he decidido hacerla AActor ya que los AActors ya serializan correctamente un array de ABaseEnemies.

Los enemigos que componen esta ronda se colocan en el mundo y luego se asignan al array de (enemiesPawns). Al iniciarse el juego, todos estos enemigos despawnearan a la espera de que 
la CombatZone mande spawnearlos. Cuando spawneen, cada vez que un enemigo muera se ejecutará la funcion (SingleEnemyDead) que comprueba si es el ultimo enemigo.
Si se ha derrotado al ultimo enemigo, se lanza el evento AllEnemiesDeadEvent que avisa a la CombatZone de que ha terminado la ronda

*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatRoundPoint.generated.h"

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FRoundActivated );

UCLASS()
class SCYTHER_API ACombatRoundPoint: public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACombatRoundPoint();

	class AScytherGameModeBase* gm;

	UPROPERTY( BlueprintAssignable )
		FRoundActivated CombatRoundActivated;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "CombatZoneSpawnPoint" )
		TArray<class ABaseEnemy*> enemiesPawns;

	DECLARE_DELEGATE( FAllEnemiesDead );
	FAllEnemiesDead AllEnemiesDeadEvent;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "CombatZoneSpawnPoint" )
	int enemyCount = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

	UFUNCTION( BlueprintCallable )
	void SpawnEnemies();

	UFUNCTION( BlueprintCallable )
	void DespawnEnemies();

	UFUNCTION(BlueprintCallable)
	void SingleEnemyDead( );

	UFUNCTION()
		void OnActorDestroyedDelegate( AActor* DestroyedActor );

	UFUNCTION()
	void CombatRoundActivatedBroadcast();
};
