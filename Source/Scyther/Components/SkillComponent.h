/*****************************************************************************************************/
/******************** Archivo fuente del componente que maneja las habilidades ***********************/
/****************************************** Autor: Alvaro ********************************************/
/**************************************** Fecha: 21-03-2023 ******************************************/
/*****************************************************************************************************/

//Updated by Marco 23-03-2023 to support the new player pawn
//Updated by Marco 25-03-2023 added events for the UI, a function to update the mana level and a dummy function to test combinedSkill from the UI
#pragma once

#include "fmod.hpp"
#include "FMODEvent.h"
#include "FMODBlueprintStatics.h"
#include <FMODAudioComponent.h>
#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"

#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

#include "../Player/ScytherPlayerPawn.h"
#include "../Player/Habilidades/LightSkillAttack.h"
#include "SkillComponent.generated.h"

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FnotEnoughtMana, bool, isLightMana );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FManaFull, bool, isLightMana );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams( FManaChanged, bool, isLightMana, float, oldAmount, float, newAmount );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FSimpleManaSkillCast, bool, isLightMana );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FDoubleManaSkillCast);

//Currently theres no cooldown
//UDELEGATE( BlueprintAuthorityOnly )
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FManaSkillAvailableAgain, bool, isLightMana );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FBothManaFull);


UENUM( BlueprintType, meta = ( DisplayName = "Tipo de habilidad" ) )
enum class SkillType: uint8
{
	NONE UMETA( DisplayName = "None" ),
	LIGHT UMETA( DisplayName = "Habilidad de luz" ),
	DARK UMETA( DisplayName = "Habilidad de oscuridad" ),
};

UCLASS()
class SCYTHER_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:

	//--- Delegates ---//
	UPROPERTY( BlueprintAssignable )
	FManaFull manaFullEvent;

	UPROPERTY( BlueprintAssignable )
	FBothManaFull bothManaFullEvent;

	UPROPERTY( BlueprintAssignable )
	FManaChanged manaChangedEvent;

	UPROPERTY( BlueprintAssignable)
	FnotEnoughtMana notEnoughtManaEvent;

	UPROPERTY( BlueprintAssignable )
	FSimpleManaSkillCast simpleManaSkillCast;

	UPROPERTY( BlueprintAssignable )
	FDoubleManaSkillCast  DoubleManaSkillCastEvent;

	//-- FOR UI BLUEPRINT USE --//

	/// <value> Current light mana of the player, changes on execution </value>
	UPROPERTY( BlueprintReadOnly )
		float currentLightMana = 0;
	/// <value> Current dark mana of the player, changes on execution </value>
	UPROPERTY( BlueprintReadOnly )
		float currentDarkMana = 0;


	int actualLightSkillAttacks = 0;
	int lightSkillAttacks = 0;


	/// <value> If the player is currently on god mode </value>
	bool isGodMode = false;

private:

	/// <value>The character< / value>
	AScytherPlayerPawn* character = nullptr;
	/// <value>The skill that is active</value>
	SkillType skillState;
	/// <value>True if the dark skill is currently atracting enemies</value>
	bool isDarkSkillAtracting = false;
	/// <value>True if the dark skill is currently pushing enemies</value>
	bool isDarkSkillPushing = false;
	/// <value> List of enemies that are being atracted or pushed </value> 
	TArray<AActor*> enemiesAffected;
	/// <value>Controls the delay of the skill</value>
	FTimerHandle SkillTimerHandle;
	/// <value>Controls the delay of the dark skill atraction</value>
	FTimerHandle DarkSkillTimerHandle;

	float actualLightSkillCooldown;

public:

	//------------------------//
	//      COMPONENTS
	//------------------------//



	/// <value> Cost of the light skill </value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Light Skill", meta = (
		ToolTip = "Coste de la habilidad" ) )
		float lightSkillCost = 10.f;
	/// <value> Duration of the light skill </value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Light Skill", meta = (
		ToolTip = "Duracion de la habilidad" ) )
		float lightSkillCooldown = 2.f;
	/// <value> Damage of the light projectile </value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Light Skill", meta = (
		ToolTip = "Daño del proyectil de luz" ) )
		float lightProjectileDamage = 5.f;
	/// <value> Width for the light projectile </value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Light Skill", meta = (
		ToolTip = "Anchura del proyectil de luz" ) )
		float lightProyectileWidth = 200.f;
	/// <value> Height for the light projectile </value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Light Skill", meta = (
		ToolTip = "Anchura del proyectil de luz" ) )
		float lightProjectileHeight = 40.f;
	/// <value> Speed for the light proyectile </value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Light Skill", meta = (
		ToolTip = "Velocidad del proyectil de luz" ) )
		float lightProyectileSpeed = 1000.f;
	/// <value>Duration of the light proyectile </value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Light Skill", meta = (
		ToolTip = "Duracion del proyectil de luz" ) )
		float lightProyectileDuration = 1.f;



	/// <value>Cost of the dark skill</value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Dark Skill", meta = (
		ToolTip = "Coste de la habilidad" ) )
		float darkSkillCost = 10.f;
	/// <value>Cost of the dark skill</value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Dark Skill", meta = (
		ToolTip = "Daño de la habilidad" ) )
		float darkSkillDamage = 15.f;
	/// <value>Atract force of the dark skill</value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Dark Skill", meta = (
		ToolTip = "Fuerza de atracction de la habilidad" ) )
		float darkSkillAtractStrenght = 5.f;
	/// <value>Push force of the dark skill</value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Dark Skill", meta = (
		ToolTip = "Fuerza de empuje de la habilidad" ) )
		float darkSkillPushStrenght = 15.f;
	/// <value>Range of action for the dark skill</value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Dark Skill", meta = (
		ToolTip = "Rango de accion de la habilidad" ) )
		float darkSkillRange = 600.f;
	/// <value>Range of action for the attack of the dark skill</value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Dark Skill", meta = (
		ToolTip = "Rango de accion del ataque" ) )
		float darkSkillAttackRange = 300.f;
	/// <value>Range for atracting enemies</value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Dark Skill", meta = (
		ToolTip = "Rango maximo hasta el que se atraen los enemigos. Para crear una separación entre estos y el jugador y que no reciba golpes." ) )
		float darkSkillSafeRange = 200.f;
	/// <value>Duration of the atraction</value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Dark Skill", meta = (
		ToolTip = "Duracion de la atraccion" ) )
		float darkSkillAtractionDuration = 1.f;




	/// <value>Maximun light mana of the player</value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Skill", meta = (
		ToolTip = "Mana de luz maximo del jugador" ) )
		float maxLightMana = 100.f;
	/// <value>Maximun dark mana of the player</value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Skill", meta = (
		ToolTip = "Mana de oscuridad maximo del jugador" ) )
		float maxDarkMana = 100.f;
	/// <value>Initial light mana of the player</value>
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Design | Skill", meta = (
		ToolTip = "Mana de luz inicial del jugador" ) )
		float initialLightMana = 0;
	/// <value>Initial dark mana of the player</value>
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Design | Skill", meta = (
		ToolTip = "Mana de oscuridad inicial del jugador" ) )
		float initialDarkMana = 0;


	/// <value> Light Skill active VFX </value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ART | VFX", meta = (
		ToolTip = "VFX para la habilidad de luz" ) )
		UNiagaraSystem* lightSkillAtractionVFX;
	/// <value> Light Skill attack VFX </value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ART | VFX", meta = (
		ToolTip = "VFX para el ataque de la habilidad de luz" ) )
		UNiagaraSystem* lightSkillAttackVFX;
	/// <value> Light Skill Hit Attack VFX </value>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ART | VFX", meta = (
		ToolTip = "VFX para el ataque de la habilidad de luz" ) )
		UNiagaraSystem* lightSkillFlashHitVFX;




	/// <value> DarkSkill Animation Montage </value>
	UPROPERTY( EditAnywhere, Category = "Art | Animation", meta = (
		ToolTip = "Dark Skill Attack Anim Montaje" ) )
		UAnimMontage* DarkSkillAttackAnimMontage;
	
public:	
	/// <summary>
	/// Constructor: Sets default values for this component's properties
	/// </summary>
	USkillComponent();
	/// <summary>
	/// Tick: Called every frame
	/// </summary>
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;


	//-- LIGHT SKILL --//

	/// <summary>
	/// Logic for the launch of the light skill. Called from ScytherPlayerPawn when input is recived
	/// </summary>
	void LightSkill();
	/// <summary>
	/// Apply the light skill effects to all the currently overlapped actors in range
	/// </summary>
	void DoLightSkill();
	/// <summary>
	/// Activates the light skill and sets the timer
	/// </summary>
	void EnableLightSkill();


	//-- DARK SKILL --//

	/// <summary>
	/// Logic for the launch of the dark skill. Called from ScytherPlayerPawn when input is recived
	/// </summary>
	void DarkSkill();
	/// <summary>
	/// Apply the dark skill effects to all the currently overlapped actors in range
	/// </summary>
	void DoDarkSkill();
	/// <summary>
	/// Activates the dark skill and sets the timer
	/// </summary>
	void EnableDarkSkill();

	UFUNCTION( BlueprintCallable, Category = "Skill" )
		void OnDarkSkillDamage();

	UFUNCTION( BlueprintCallable, Category = "Skill" )
		void OnDarkSkillAtractionStart();
	
	UFUNCTION( BlueprintCallable, Category = "Skill" )
		void OnDarkSkillAtractionEnd();

	UFUNCTION( BlueprintCallable, Category = "Skill" )
		void OnDarkSkillPushStart();

	UFUNCTION( BlueprintCallable, Category = "Skill" )
		void OnDarkSkillPushEnd();

	UFUNCTION( BlueprintCallable, Category = "Skill" )
		void OnDarkSkillEnd();


	//-- GETTERS & SETTERS --//

	/// <summary>
	/// Getter for skillState
	/// </summary>
	inline SkillType GetActiveSkill() { return skillState; };
	/// <summary>
	/// Setter for skillState
	/// </summary>
	inline void SetActiveSkill( const SkillType newActiveSkill ) { skillState = newActiveSkill; };


	//-- OTHERS --//

	/// <summary>
	/// Deactivates the currently active skill
	/// </summary>
	void DisabledSkill();

	/// <summary>
	/// Updates one current mana type value by a especific amount
	/// </summary>
	UFUNCTION( BlueprintCallable)
		void UpdateMana( bool isLightMana, float amount );

	UFUNCTION( BlueprintCallable )
		bool IsOnLightSkill();

protected:

	/// <summary>
	/// BeginPlay: Called when the game starts
	/// </summary>
	virtual void BeginPlay() override;	
};
