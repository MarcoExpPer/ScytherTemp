// Fill out your copyright notice in the Description page of Project Settings.

/*
Autor: Marco Expósito Pérez.

Last Update: 23/01/2023

File Overview:
Este componente controla la vida de una entidad y avisa de los cambios en esta.
----------
PARAMETROS
----------
Tiene tres parametros para controlar la vida actual del personaje.
- current health: La vida que tiene el la entidad en un momento concreto.
- initial health: La vida que tiene la entidad al instanciarse.
- max health: El valor maximo que puede alcanzar tanto la vida actual como la incial

Tiene un mapa llamado Vulnerabilities | vulnerabilityMap que controla la vulnerabilidad de la entidad a cada tipo de daño.
Si un tipo no esta definido, se predetermina a valor 1.
Un valor de 1 implica que el daño no cambia. Un valor de 0 que no recibe daño de ese tipo. Un valor negativo cura a la entidad.
 Un valor mayor que 1 aumenta el daño recibido

Tiene dos delegados a los que cualquiera puede bindearse.
- healthChangedEvent se lanza cada vez que la vida de la entidad se ha intentado modificar. Tiene tres parametros,
la vida antes del cambio, la vida actual y el tipo del daño
- Zero health event se lanza cuando la vida de la entidad ha llegado a cero. Tiene un parametro que es el tipo de daño.

----------
FUNCIONES
----------
- Change Health. Cambia la vida de la entidad ignorando las vulnerabilidades. Mas informacion en la declaracion de la funcion.
- Receive Damage. Cambia la vida de la entidad aplicando las vulnerabilidades. Mas informacion en la declaracion de la funcion.

*/
#pragma once
// 1- Includes
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Player/ScytherPlayerPawn.h"
#include "AttackComponent.h"
#include "HealthComponent.generated.h"
// 2- Defines
// 3- Using

// 4- Clases y estructuras auxiliares
/// <summary>
/// Enumerado que representa todos los tipos posibles de daño que cualquier entidad puede sufrir. 
/// No llamarlo DamageTypes o Unreal crashea porque ese enumerado ya existe.
/// </summary>
UENUM( BlueprintType, meta = ( DisplayName="Damage types"))
enum class DamageModes: uint8
{
	DASHEABLE UMETA( DisplayName = "DASHEABLE" ),
	INEVITABLE UMETA( DisplayName = "INEVITABLE" ),
	DEADZONE  UMETA( DisplayName = "DEADZONE" ),
	HEALING  UMETA( DisplayName = "HEALING" ),
};

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams( FHealthChangedEvent, float, oldHp, float, newHp, DamageModes, damageType );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FZeroHealthEvent, DamageModes, damageType );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FInmuneHit, DamageModes, damageType );

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class SCYTHER_API UHealthComponent: public UActorComponent
{
	GENERATED_BODY()

		// 5- Variables

private:

	FTimerHandle InvencibilityTimerHandle;
	bool invencible = false;


public:
	//-- Vida --//
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "HealthComponent | Health", meta = ( DisplayName = "current health",
		ToolTip = "Vida actual de la entidad. En el constructor se inicializa a la vida actual. Solo es editable a traves de funciones" ) )
		float currentHp;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "HealthComponent | Health", meta = ( DisplayName = "Initial health",
		ToolTip = "Vida inicial de la entidad. Valor que se adjudica a la vida actual cuando se instancia esta unidad.",
		ClampMin = 0, UIMin = 0 ) )
		float initialHp = 55;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "HealthComponent | Health", meta = ( DisplayName = "Max health",
		ToolTip = "Vida maxima de la entidad. Su vida nunca podra ser mayor que este valor.",
		ClampMin = 0, UIMin = 0 ) )
		float maxHp = 60;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "HealthComponent | Invencibility", meta = ( DisplayName = "Invencibility time",
		ToolTip = "Cuando se reciba daño, el personaje se hará invencible por este tiempo. (segundos)",
		ClampMin = 0, UIMin = 0 ) )
		float invencibilityTime = 0.5;

	//-- Vulnerabilidades --//
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "HealthComponent | Vulnerabilities", meta = ( DisplayName = "Vulnerabilities",
		ShowOnlyInnerProperties,
		ToolTip = "Decide a que tipos de daño el jugador es inmune" ) )
		TMap<DamageModes, bool> inmunities;

	//-- Delegates --//
	UPROPERTY( BlueprintAssignable, Category = "HealthComponent | Events", meta = ( ToolTip = "Ejecutado cuando la vida de la entidad varía." ) )
		FHealthChangedEvent healthChangedEvent;

	UPROPERTY( BlueprintAssignable, Category = "HealthComponent | Events", meta = ( ToolTip = "Ejecutado cuando la vida de la entidad llega a cero." ) )
		FZeroHealthEvent zerohealthEvent;

	UPROPERTY( BlueprintAssignable, Category = "HealthComponent | Events", meta = ( ToolTip = "Ejecutado cuando la entidad recibe daño de un Modo al que es inmune." ) )
		FInmuneHit inmuneHitEvent;

	bool isGodMode = false;

	bool saveLastHp = false;
	// 6- Funciones

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void InitInmunities( DamageModes mode );
public:
	// Sets default values for this component's properties
	UHealthComponent();

	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UFUNCTION( BlueprintCallable, Category = "HealthComponent", meta = ( DisplayName = "Change health",
		ToolTip = "Cambia la vida actual de la entidad en un valor determinado por el parametro healthOffset ignorando las vulnerabilidades. Si sendEvent esta a true, lanza un evento con la anterior vida, la nueva vida actual y el tipo de daño como parametros" ) )
		/// <summary>
		/// Cambia la vida del jugador sin reducirla mas de 0 o aumentarla mas de la vida maxima. Ignora las vulnerabilidades de la entidad.
		/// Si sendEvent vale true, lanza un evento avisando de que la vida ha cambiado, indicando cuanto valia antes y cuanto vale 
		/// ahora y el tipo del daño.
		/// El tipo de daño solo es necesario para mandar el evento, ante la duda, dejarlo a NEUTRAL.
		/// Si la vida llega a cero. Lanza un evento avisando de ello, independiente de la variable sendEvent.
		/// </summary>
		/// <param name="healthOffset"></param>
		/// <param name="sendEvent"></param>
		/// <param name="type"></param>
		void changeHealth( float healthOffset, DamageModes type, bool sendEvent = true );

	UFUNCTION( BlueprintCallable, Category = "HealthComponent", meta = ( DisplayName = "Receive Damage",
		ToolTip = "Cambia la vida actual de la entidad en un valor determinado por el parametro damage y aplicando a las vulnerabilidades. Si sendEvent esta a true, lanza un evento con la anterior vida, la nueva vida actual y el tipo de daño como parametros" ) )
		/// <summary>
		/// Reduce la vida del jugador segun el valor de damage. Si damage es negativo, la aumentara. Utiliza las vulnerabilidades 
		/// de la entidad para modificar el dano recibido.
		/// Si sendEvent vale true, lanza un evento avisando de que la vida ha cambiado, indicando cuanto valia antes y cuanto vale 
		/// ahora y el tipo del daño.
		/// Si la vida llega a cero. Lanza un evento avisando de ello, independiente de la variable sendEvent.
		/// </summary>
		/// <param name="healthOffset"></param>
		/// <param name="sendEvent"></param>
		/// <param name="type"></param>
		void receiveDamage( float damage, DamageModes type = DamageModes::DASHEABLE, bool sendEvent = true );

	/// <summary>
	/// Use it to activate this component god mode. Making the user inmune to dasheable and inevitable attacks
	/// </summary>
	UFUNCTION( BlueprintCallable )
	void becomeGod();
	/// <summary>
	/// Use it to remove this component god mode. Making the user vulnerable to dasheable and inevitable attacks
	/// </summary>
	UFUNCTION( BlueprintCallable )
	void becomeMortal();

	/// <summary>
	/// Function to make this entity inmune to dasheable attacks
	/// </summary>
	void enableDashInmune();
	/// <summary>
	/// Function to make this entity vulnerable to dasheable attacks
	/// </summary>
	void disableDashInmune();
};