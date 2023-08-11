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
#include "../Components/TargetMarkComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "BaseEnemy.generated.h"

class ACombatRoundPoint;

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FTurnLightEvent );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FTurnDarkEvent );


UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FTurningDarkEvent, float, timeToTurn );


UCLASS()
class SCYTHER_API ABaseEnemy: public APawn, public ICanBeHit, public ICanEnterDarkLightAreas, public ICanBeZtargeted
{
	GENERATED_BODY()

public:
	//-- COMPONENTS --//
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Base Enemy | Components" )
		class UHealthComponent* healthComp;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Conejo | Components", meta = ( DisplayName = "Target Mark Component" ) )
		UTargetMarkComponent* targetMarkComp;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Base Enemy | Components" )
		class UDropLooteablesComponent* dropLooteablesComp;

	//-- DELEGATES --//
public:
	UPROPERTY( BlueprintAssignable, Category = "Base Enemy | Events", meta = ( DisplayName = "Turned into light mode",
		ToolTip = "Ejecutado cuando la entidad se pasa al modo luz." ) )
		FTurnLightEvent turnLightEvent;

	UPROPERTY( BlueprintAssignable, Category = "Base Enemy | Events", meta = ( DisplayName = "Turned into dark mode",
		ToolTip = "Ejecutado cuando la entidad se pasa al modo oscuridad." ) )
		FTurnDarkEvent turnDarkEvent;

	UPROPERTY( BlueprintAssignable, Category = "Base Enemy | Events" )
	FTurningDarkEvent turningDarkEvent;

	//-- BALANCE --/
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Base Enemy | Balance | Deprecated", meta = ( DisplayName = "Light To shadow timer",
		ToolTip = "Tiempo (s) que tarda un enemigo de luz en pasar al estado sombra" ) )
		float maxLightToShadowTimer = 5;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Base Enemy | Balance | Deprecated", meta = ( DisplayName = "Shadow To light timer",
		ToolTip = "Tiempo que tarda un enemigo de sombra en pasar al estado luz" ) )
		float maxShadowToLightTimer = 10;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Base Enemy | Balance", meta = ( DisplayName = "isLightMode",
		ToolTip = "Si esta a true, el enemigo esta en modo luz. Si esta a false, esta en modo oscuridad" ) )
		bool isLightMode = true;

	//Chance of a dark enemy of giving health as loot. If it turns out it doesnt give health, it will give half of the light mana amount
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Base Enemy | Loot")
		float healthChance = 0.4f;

	//Chance of a dark enemy of giving health as loot. If it turns out it doesnt give health, it will give half of the light mana amount
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Base Enemy | Loot" )
		int lightManaAmount = 15;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Base Enemy | Loot" )
		int lightManaCrystals = 3;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Base Enemy | Loot" )
		int healAmount = 10;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Base Enemy | Loot" )
		int healCrystals = 1;

private:
	bool originalLightMode = true;
public:

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Base Enemy | Balance" )
		bool changeToDarkModeAfterSomeTime = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Base Enemy | Balance" )
		float timeToChangeToDark = 10;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Base Enemy | Balance | Deprecated", meta = (
		ToolTip = "Si esta a true, el enemigo esta siendo afectado por una habilidad y pisar areas de luz/oscuridad no tendra efecto mientras este en el area del jugador" ) )
		bool isAfectedBySkill = false;



	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Base Enemy | Balance | Deprecated", meta = (
		ToolTip = "Indica si el enemigo puede o no ser afectado por la habilidad de luz. Si está a false, esta no tendrá ningun efecto sobre el enemigo" ) )
		bool isAfectedByLightSkill = true;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Base Enemy | Balance | Deprecated", meta = (
		ToolTip = "Indica si el enemigo puede o no ser afectado por la habilidad de oscuridad. Si está a false, esta no tendrá ningun efecto sobre el enemigo" ) )
		bool isAfectedByDarkSkill = true;



	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "VFX" )
		UNiagaraSystem* DeadVFX;
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "VFX" )
		UNiagaraSystem* LightSpawnVFX;
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "VFX" )
		UNiagaraSystem* DarkSpawnVFX;

	class ACombatRoundPoint* ThisRound;

protected:

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Base Enemy | Balance", meta = ( DisplayName = "isInLightArea",
		ToolTip = "Si esta a true, el enemigo esta en un area de luz" ) )
		bool isInLightArea = true;
	//-- SPAWN ON DEATH --/
public:

	//El tracker del tiempo para transformarse
	float currentTransformationTimer = 0;

	//This is a safecheck bool to not make light enemies in light areas continuosly check if they should swap to light mode
	bool isTransforming = false;

	// To check if is in the ground the actor when is knockbacking
	FHitResult groundedHit;

	bool isHiteable = true;

	UPROPERTY( EditAnywhere, BlueprintReadWrite)
	bool isDead = false;
private:

	bool isHitted = false;
	bool isMoving = false;
	FVector characterLocationWhenReceiveHit;
	FVector targetLocationWhenIsHitted;
	AttackState typeAttackReceived;

	FTimerHandle changeToDarkTimerHandle;
	//-- ATTACK --//
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Base Enemy | Attack", meta = ( DisplayName = "base Damage",
		ToolTip = "Daño base del enemigo" ) )
		float baseDamage = 10;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Base Enemy | Attack", meta = ( DisplayName = "isAttacking",
		ToolTip = "Si esta a true, el enemigo esta realizando un ataque" ) )
		bool isAttacking = false;

	UPROPERTY( EditAnywhere,
	Category = "Base Enemy | Knockback | Basic", meta = (
		DisplayName = "Speed",
		ToolTip = "Velocidad a la que se desplaza al ser golpeado. Si es 0, el enemigo no hara el retroceso" ) )
		float speedKnockback = 800.f;

	UPROPERTY( EditAnywhere,
	Category = "Base Enemy | Knockback | First Attack ", meta = (
		DisplayName = "Push Distance",
		ToolTip = "Distancia a la que empuja al enemigo al recibir al ataque inicial" ) )
		float firstAttackForceKnockback = 100.f;

	UPROPERTY( EditAnywhere,
	Category = "Base Enemy | Knockback | Second Attack", meta = (
		DisplayName = "Push Distance",
		ToolTip = "Distancia a la que empuja al enemigo al recibir el segundo golpe" ) )
		float secondAttackForceKnockback = 100.f;

	UPROPERTY( EditAnywhere,
	Category = "Base Enemy | Knockback | Final Attack", meta = (
		DisplayName = "Push Distance",
		ToolTip = "Distancia a la que empuja al enemigo al recibir el ataque final" ) )
		float finalAttackForceKnockback = 150.f;

	UPROPERTY( EditAnywhere,
	Category = "Base Enemy | Knockback | Air Attack", meta = (
		DisplayName = "Push distance Scythe",
		ToolTip = "Distancia a la que empuja al enemigo al recibir el ataque aereo" ) )
		float airAttackForceKnockback = 75.f;
	UPROPERTY( EditAnywhere,
		Category = "Base Enemy | Knockback | Air Attack", meta = (
			DisplayName = "Push Distance Body",
			ToolTip = "Distancia a la que empuja al enemigo con la colision con el player mientras cae" ) )
		float airAttackBodyHitForceKnockback = 150.f;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Base Enemy" )
		bool isInCombatZone = false;

	//-- SPAWN --//
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Base Enemy" )
		FTransform initialTransform;

public:
	// Sets default values for this pawn's properties
	ABaseEnemy();

	UFUNCTION( BlueprintCallable, Category = "Base Enemy | On death", meta = (
		ToolTip = "Genera el loot correspondiente a este enemigo" ) )
		void GenerateLoot();

	UFUNCTION( BlueprintCallable, Category = "Base Enemy", meta = ( DisplayName = "Spawn" ) )
		virtual void spawn( FTransform spawnTransform );

	UFUNCTION( BlueprintCallable, Category = "Base Enemy", meta = ( DisplayName = "DeSpawn" ) )
		virtual void deSpawn();

	UFUNCTION( BlueprintCallable, Category = "Base Enemy", meta = ( DisplayName = "turn dark mode",
		ToolTip = "Modifica todo lo necesario para que este enemigo se ponga en modo oscuridad" ) )
		void turnDarkMode();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION( BlueprintCallable, Category = "Base Enemy", meta = ( DisplayName = "turn light mode",
		ToolTip = "Modifica todo lo necesario para que este enemigo se ponga en modo luz" ) )
		void turnLightMode();



	UFUNCTION()
		void whenHpGoesTo0( DamageModes type );

	UFUNCTION()
		void WhenIsHitted( FString name, FVector characterLocation, AttackState typeAttack );
public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent ) override;

	virtual void DoDirectDamage_Implementation( float damage ) override;

	virtual void EnterDarkArea_Implementation() override;
	virtual void EnterLightArea_Implementation() override;
	virtual void SkillToLightMode_Implementation() override;
	virtual void SkillToDarkMode_Implementation() override;
	virtual void SkillEnds_Implementation() override;

	UFUNCTION()
	void roundStartEventExecution();
	UFUNCTION()
	void bindToRoundStart( ACombatRoundPoint* round );

	UFUNCTION( BlueprintCallable )
		void toggleAI( bool activate );


	void CalculateTargetPositionWhenIsHitted();
	void KnockbackWhenIsHitted( float deltaTime );

	//GETTERS
	bool GetIsDead();
	bool GetIsInCombatZone();
};
