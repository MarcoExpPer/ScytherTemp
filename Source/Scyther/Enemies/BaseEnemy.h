// Fill out your copyright notice in the Description page of Project Settings.

/*
Autor: Marco Expósito Pérez.

Last Update: 11/02/2023

File Overview:
Este Actor es un pawn generico con la funcionalidad basica que tienen todos los enemigos
----------
PARAMETROS
----------
[BALANCE]
- balanceLevel: (float) El nivel actual de equilibrio.
- maxBalanceLevel: (float) El nivel maximo de equilibrio
- isLightMode: (bool) Si true, el enemigo esta en modo luz, si no, esta en modo oscuridad.

[ATTACK]
- isAttacking: (bool) Si true, el enemigo esta atacando.
- baseDamage: (float) Daño base del enemigo.

----------
DELEGATES
----------
- turnLightEvent (sin parametros) Se lanza cuando el enemigo se ha transformado en su modo luz.
- turnDarkEvent (sin parametros) Se lanza cuando el enemigo se ha transformado en su modo oscuridad.

----------
FUNCIONES
----------
- updateBalanceLevel. (float extraBalance) Actualiza el nivel de equilibrio teniendo en cuenta el maximo y el minimo.
- turnLightMode. () Convierte al enemigo a su modo luz y llama al delegate correspondiente.
- turnDarkMode. () Convierte al enemigo a su modo luz y llama al delegate correspondiente.

*/


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "../Interfaces/CanBeHit.h"
#include "../Interfaces/CanEnterDarkLightAreas.h"
#include "../Interfaces/CanBeZtargeted.h"
#include <Scyther/Components/HealthComponent.h>
#include "BaseEnemy.generated.h"



UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FTurnLightEvent );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FTurnDarkEvent );

UCLASS()
class SCYTHER_API ABaseEnemy : public APawn, public ICanBeHit, public ICanEnterDarkLightAreas, public ICanBeZtargeted
{
	GENERATED_BODY()

public:
	//-- COMPONENTS --//
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true" ))
		UHealthComponent* healthComp;

	//-- DELEGATES --//
public:
	UPROPERTY( BlueprintAssignable, Category = "Events", meta = ( DisplayName = "Turned into light mode",
		ToolTip = "Ejecutado cuando la entidad se pasa al modo luz." ) )
		FTurnLightEvent turnLightEvent;

	UPROPERTY( BlueprintAssignable, Category = "Events", meta = ( DisplayName = "Turned into dark mode",
		ToolTip = "Ejecutado cuando la entidad se pasa al modo oscuridad." ) )
		FTurnDarkEvent turnDarkEvent;
	//-- BALANCE --/
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = ( DisplayName = "Light To shadow timer",
		ToolTip = "Tiempo (s) que tarda un enemigo de luz en pasar al estado sombra" ) )
		float maxLightToShadowTimer = 5;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = ( DisplayName = "Shadow To light timer",
		ToolTip = "Tiempo que tarda un enemigo de sombra en pasar al estado luz" ) )
		float maxShadowToLightTimer = 10;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = ( DisplayName = "isLightMode",
		ToolTip = "Si esta a true, el enemigo esta en modo luz. Si esta a false, esta en modo oscuridad" ) )
		bool isLightMode;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Balance", meta = (
		ToolTip = "Si esta a true, el enemigo esta siendo afectado por una habilidad y pisar areas de luz/oscuridad no tendra efecto mientras este en el area del jugador" ) )
		bool isAfectedBySkill = false;

protected:

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Balance", meta = ( DisplayName = "isInLightArea",
		ToolTip = "Si esta a true, el enemigo esta en un area de luz" ) )
		bool isInLightArea = true;
	//-- SPAWN ON DEATH --/
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "OnDeath", meta = (
		ToolTip = "Cantidad de dropeables de mana que spawnean cuando el enemigo muere." ) )
		float crystalsAmount = 10;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "OnDeath", meta = (
		ToolTip = "El numero de puntos de experiencia que va a conseguir el jugador al recoger TODOS los dropeables." ) )
		float experiencePoints = 10;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "OnDeath", meta = (
		ToolTip = "El numero de mana de luz que va a conseguir el jugador al recoger TODOS los dropeables." ) )
		float lightDrops = 15;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "OnDeath", meta = (
		ToolTip = "El numero de mana de oscuridad que va a conseguir el jugador al recoger TODOS los dropeables." ) )
		float darkDrops = 15;


	//El tracker del tiempo para transformarse
	float currentTransformationTimer = 0;

	//This is a safecheck bool to not make light enemies in light areas continuosly check if they should swap to light mode
	bool isTransforming = false;
private:
	bool isDead = false;

	//-- ATTACK --//
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = ( DisplayName = "base Damage",
		ToolTip = "Daño base del enemigo" ) )
		float baseDamage = 10;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = ( DisplayName = "isAttacking",
		ToolTip = "Si esta a true, el enemigo esta realizando un ataque" ) )
		bool isAttacking = false;
protected:



public:
	// Sets default values for this pawn's properties
	ABaseEnemy();

	UFUNCTION( BlueprintCallable, Category = "On death", meta = (
		ToolTip = "Genera el loot correspondiente a este enemigo" ) )
		void GenerateLoot();

	UFUNCTION( BlueprintCallable, Category = "BaseEnemyController", meta = ( DisplayName = "Spawn" ) )
		virtual void spawn( FVector spawnPosition );

	UFUNCTION( BlueprintCallable, Category = "BaseEnemyController", meta = ( DisplayName = "DeSpawn" ) )
		virtual void deSpawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION( BlueprintCallable, Category = "Balance", meta = ( DisplayName = "turn light mode",
		ToolTip = "Modifica todo lo necesario para que este enemigo se ponga en modo luz" ) )
	void turnLightMode();

	UFUNCTION( BlueprintCallable, Category = "Balance", meta = ( DisplayName = "turn dark mode",
		ToolTip = "Modifica todo lo necesario para que este enemigo se ponga en modo luz" ) )
	void turnDarkMode();

	UFUNCTION()
		void whenHpGoesTo0( ElementTypes type );

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void DoDirectDamage_Implementation(float damage) override;

	virtual void EnterDarkArea_Implementation() override;
	virtual void EnterLightArea_Implementation() override;
	virtual void SkillToLightMode_Implementation() override;
	virtual void SkillToDarkMode_Implementation() override;
	virtual void SkillEnds_Implementation() override;

	//GETTERS
	bool GetIsDead();

};
