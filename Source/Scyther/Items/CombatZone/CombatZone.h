/*
Autor: Marco Expósito Pérez.

Last Update: 2/05/2023

File Overview:
Esta clase controla una zona de combate para el jugador, que puede estar cerrada o abierta y donde aparecen enemigos por rondas

Cuando el jugador entra en el main trigger, la zona de combate se activa. Primero las puertas se cierran, despues, tras un delay(delayBeforeFirstRound) la primera ronda spawnea.
Ahora, cada vez que el jugador mate a todos los enemigos de una ronda, la funcion (roundFinished) se ejecutará, provocando un delay (delayBetweenRounds) antes de que spawnee la siguiente. 

Cuando todas las rondas hayan sido derrotadas, las puertas se abren y la zona de combate termina.

Las rondas estan compuestas por Actores del tipo ACombatRoundPoint mientras que las puertas son cualquier Actor que implemente la interfaz isCombatZoneDoor con sus dos funciones,
CloseCombatZoneDoor y OpenCombatZoneDoor.
*/

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatZone.generated.h"

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FCombatStarts );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FCombatEnds );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FRoundFinished );

UCLASS()
class SCYTHER_API ACombatZone: public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACombatZone();

	//-- Components --//
	
	/// <summary>
	/// Cuando se entra en este trigger, la zona de combate se activa
	/// </summary>
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UBoxComponent* mainTrigger;

	//-- Events --//
	UPROPERTY( BlueprintAssignable )
		FCombatStarts CombatStartsEvent;
	UPROPERTY( BlueprintAssignable )
		FCombatEnds CombatEndsEvent;
	UPROPERTY( BlueprintAssignable )
		FRoundFinished CombatRoundFinished;

	//-- Doors --//

	/// <summary>
	/// Estos actores deben implementar la interfaz IsCombatZoneDoor
	/// </summary>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "CombatZone | Doors", meta = ( MustImplement = "isCombatZoneDoor" ) )
		TArray<AActor*> doors;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "CombatZone | Doors" )
		bool destroyDoorsOnCombatZoneDestroy = true;


	//-- Rounds --//
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "CombatZone" )
		TArray<class ACombatRoundPoint*> rounds;

	//-- Parameters --/
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "CombatZone | Delays" )
		float delayBetweenRounds = 1.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "CombatZone | Delays" )
		float delayBeforeFirstRound = 2.f;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		bool firstRoundStartsSpawned = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		bool autoRespawnOnCombatEnd = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		bool externalActivation = false;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
		int currentRound = 0;

private:
	bool isCombatActive = false;


	FTimerHandle startRoundHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

	UFUNCTION()
		void OnCombatTriggerOverlap( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	UFUNCTION()
		void roundFinished();

	UFUNCTION( BlueprintCallable )
		void respawnCombatZone();

	UFUNCTION()
		void OnActorDestroyedDelegate( AActor* DestroyedActor );

	UFUNCTION( BlueprintCallable )
	void startCombat();

	UFUNCTION( BlueprintCallable )
	void endCombat( bool allowAutoRespawn = false );

private:


	void StartRound();

	void CloseDoors();
	void OpenDoors();
	void PrepareDoors();

	void ValidateDoors();
};
