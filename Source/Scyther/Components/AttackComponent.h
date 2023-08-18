
/*****************************************************************************************************/
/***************** Archivo fuente del componente que maneja el ataque del caracter *******************/
/****************************************** Autor: Alvaro ********************************************/
/****************************************** Autor: Miguel ********************************************/
/**************************************** Fecha: 24-03-2023 ******************************************/
/*************************************** Updated: 28-05-2023 *****************************************/
/*****************************************************************************************************/
#pragma once

// 1- Includes
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Player/ScytherPlayerPawn.h"
#include "GameFramework/PlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/ForceFeedbackEffect.h"

#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

#include "AttackComponent.generated.h"

UENUM( BlueprintType, meta = ( DisplayName = "Attack State" ) )
enum class AttackState: uint8
{
	NONE UMETA( DisplayName = "No ataque" ),
	FIRST UMETA( DisplayName = "Primer Ataque" ),
	SECOND UMETA( DisplayName = "Segundo Ataque" ),
	THIRD UMETA( DisplayName = "Ataque Final" ),
	AIR UMETA( DisplayName = "Ataque Aereo" ),
	BODY UMETA( DisplayName = "Ataque Con Cuerpo" ),
};

UENUM( BlueprintType, meta = ( DisplayName = "Attack Direction" ) )
enum class AttackDirection: uint8
{
	HORIZONTAL UMETA( DisplayName = "Horizontal" ),
	VERTICAL UMETA( DisplayName = "Vertical" ),
};

UENUM( BlueprintType, meta = ( DisplayName = "SHift type" ) )
enum class ShiftType: uint8
{
	NO_SHIFT,
	SHIFT_AND_MOVE,
	SHIFT_AND_ROTATE
};

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FAttackFinished );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams( FHittedEnemy, FString, enemyName, FVector, characterLocation, AttackState, typeAttack );

/// <summary>
/// </summary>
/// <seealso cref="UActorComponent" />
UCLASS()
class SCYTHER_API UAttackComponent: public UActorComponent
{
	GENERATED_BODY()

private:
	/// The character
	AScytherPlayerPawn* character = nullptr;
	/// Mesh which plays the animation
	USkeletalMeshComponent* meshCharacter = nullptr;

	/// SkillComponent reference
	USkillComponent* skillComponent;

	/// Mesh which cointains the scythe
	UAnimInstance* meshAnimation = nullptr;
	///
	UBoxComponent* scytheAttackCol = nullptr;
	UCapsuleComponent* verticalAttackCol = nullptr;

	class ABaseEnemy* enemyToShiftTo = nullptr;
	ShiftType shiftState = ShiftType::NO_SHIFT;

	

	///
	float actualComboCooldown = 1.f;
	/// 
	TArray<FString> damagedEnemies;
	/// 
	FVector boxColOffset = FVector( 0, 0, 0 );
	///
	bool isDroping = false;
	///
	float shiftedDist = 0.f;

	float attackingStateCooldown = 0.65f;

	// ATTACK DEBUG TOOLS
private:
	bool isDoingDamage = false;

	APlayerController *controllerP;

public:
	UPROPERTY( BlueprintAssignable)
	FAttackFinished attackFinishedEvent;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | DEBUG " )
		bool attack1AreaDebug = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | DEBUG " )
		bool attack2AreaDebug = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | DEBUG " )
		bool attack3AreaDebug = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | DEBUG " )
		bool attackAirAreaDebug = false;

	/// <summary>
	/// Return the state of the attack process, related to combo processes.
	/// </summary>
	UPROPERTY( EditAnywhere, BlueprintReadWrite)
	AttackState state;
	/// <summary>
	/// Accumulator that returns the number of times the player attacks, up to a maximum of 3 times
	/// </summary>
	int attackCounter;
	/// <summary>
	/// Returns true when the character is playing any attack animation at the moment.
	/// </summary>
	bool isAttacking;
	/// <summary>
	/// Returns true when the character has just performed a series of attacks that allow a combo.
	/// </summary>
	bool isComboActivate;
	///
	float attackFinishCooldown = 0.f;
	///
	float actualPenalty = 0.f;

	bool attackMissed = false;

	/// <summary>
	/// Activate or desactivate the GOD Mode 
	/// </summary>
	bool isGodMode = false;

	UPROPERTY( BlueprintAssignable, Category = "Attack | Events", meta = ( DisplayName = "Enemyhitted",
		ToolTip = "Ejecutado cuando la guadana ha golpeado a un enemigo" ) )
		FHittedEnemy hittedEnemyEvent;

	/// <summary>
	/// FIRST ATTACK
	/// </summary>
	
	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | First Attack | Damage", meta = (
			DisplayName = "Damage",
			ToolTip = "Damage del primer ataque" ) )
		float firstAttackDamage = 10.f;

	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | First Attack | Box", meta = (
			DisplayName = "Alcance",
			ToolTip = "Alcance del primer ataque" ) )
		float firstAttackReach = 200.f;
	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | First Attack | Box", meta = (
			DisplayName = "Anchura",
			ToolTip = "Anchura del primer ataque" ) )
		float firstAttackWidth = 100.f;
	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | First Attack | Box", meta = (
			DisplayName = "Grosor",
			ToolTip = "Grosor del primer ataque" ) )
		float firstAttackThickness = 60.f;
	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | First Attack | Box", meta = (
			DisplayName = "Offset",
			ToolTip = "Offset del primer golpe" ) )
		FVector firstAttackOffset;

	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | First Attack | Combo", meta = (
			DisplayName = "Cooldown combo finish",
			ToolTip = "Cooldown para comenzar el siguiente combo cuando el combo se finaliza en el primer ataque." ) )
		float firstAttackFinishCooldown = 0.2f;
	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | First Attack | Combo", meta = (
			DisplayName = "Corta el combo?",
			ToolTip = "Si el primer ataque falla, se corta el combo" ) )
		bool firstAttackBreaksCombo = false;
	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | First Attack | Combo", meta = (
			DisplayName = "Damage rompe el combo",
			ToolTip = "Si se recibe dano, se corta el combo y la animacion" ) )
		bool firstAttackDamageBreaksCombo = true;

	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | First Attack | Failing Attack Penalty", meta = (
			DisplayName = "Tiene penalizacion?",
			ToolTip = "Si el primer ataque falla, hay penalización de tiempo" ) )
		bool firstAttackHasPenalty = false;
	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | First Attack | Failing Attack Penalty", meta = (
			DisplayName = "Penalizacion",
			ToolTip = "Tiempo de penalizacion por fallar el primer ataque" ) )
		float firstAttackPenalty = 0.5f;

	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | First Attack | Attack Forward Shift", meta = (
			DisplayName = "Distancia",
			ToolTip = "Distancia que se desplaza el jugador antes de atacar " ) )
		float firstAttackShiftDistance = 75.f;
	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | First Attack | Attack Forward Shift", meta = (
			DisplayName = "Velocidad",
			ToolTip = "Velocidad a la que se desplaza el jugador antes de atacar, medida en unidades por segundo" ) )
		float firstAttackShiftSpeed = 500.f;

	UPROPERTY( EditAnywhere,
Category = "Design | Attack | First Attack | Attack Forward Shift", meta = ( ToolTip = "Distancia minima del enemigo para shiftear hacia el" ) )
float firstAttackShiftMinDistance = 100.f;

	UPROPERTY( EditAnywhere,
Category = "Design | Attack | First Attack | Attack Forward Shift", meta = ( ToolTip = "Distancia maxima del enemigo para shiftear hacia el" ) )
float firstAttackShiftMaxDistance = 300.f;

	/// <summary>
	/// SECOND ATTACK
	/// </summary>
	
	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Second Attack | Damage", meta = (
			DisplayName = "Damage",
			ToolTip = "Damage del segundo ataque" ) )
		float secondAttackDamage = 10.f;

	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Second Attack | Box", meta = (
			DisplayName = "Alcance",
			ToolTip = "Alcance del segundo ataque" ) )
		float secondAttackReach = 200.f;
	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Second Attack | Box", meta = (
			DisplayName = "Anchura",
			ToolTip = "Anchura del segundo ataque" ) )
		float secondAttackWidth = 100.f;
	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Second Attack | Box", meta = (
			DisplayName = "Grosor",
			ToolTip = "Grosor del segundo ataque" ) )
		float secondAttackThickness = 60.f;
	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Second Attack | Box", meta = (
			DisplayName = "Offset",
			ToolTip = "Offset del segundo golpe" ) )
		FVector secondAttackOffset;

	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Second Attack | Combo", meta = (
			DisplayName = "Cooldown combo finish",
			ToolTip = "Cooldown para comenzar el siguiente combo cuando el combo se finaliza en el segundo ataque." ) )
		float secondAttackFinishCooldown = 0.5f;
	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | Second Attack | Combo", meta = (
			DisplayName = "Corta el combo?",
			ToolTip = "Si el segundo ataque falla, se corta el combo" ) )
		bool secondAttackBreaksCombo = false;
	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | Second Attack | Combo", meta = (
			DisplayName = "Damage rompe el combo",
			ToolTip = "Si se recibe dano, se corta el combo y la animacion" ) )
		bool secondAttackDamageBreaksCombo = true;

	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | Second Attack | Failing Attack Penalty", meta = (
			DisplayName = "Tiene penalizacion?",
			ToolTip = "Si el segundo ataque falla, hay penalización de tiempo" ) )
		bool secondAttackHasPenalty = false;
	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | Second Attack | Failing Attack Penalty", meta = (
			DisplayName = "Penalizacion",
			ToolTip = "Tiempo de penalizacion por fallar el segundo ataque" ) )
		float secondAttackPenalty = 0.5f;

	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | Second Attack | Attack Forward Shift", meta = (
			DisplayName = "Distancia",
			ToolTip = "Distancia que se desplaza el jugador antes de atacar " ) )
		float secondAttackShiftDistance = 100.f;
	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | Second Attack | Attack Forward Shift", meta = (
			DisplayName = "Velocidad",
			ToolTip = "Velocidad a la que se desplaza el jugador antes de atacar, medida en unidades por segundo" ) )
		float secondAttackShiftSpeed = 500.f;

	UPROPERTY( EditAnywhere,
Category = "Design | Attack | Second Attack | Attack Forward Shift", meta = ( ToolTip = "Distancia minima del enemigo para shiftear hacia el" ) )
float secondAttackShiftMinDistance = 100.f;

	UPROPERTY( EditAnywhere,
Category = "Design | Attack | Second Attack | Attack Forward Shift", meta = ( ToolTip = "Distancia maxima del enemigo para shiftear hacia el" ) )
float secondAttackShiftMaxDistance = 300.f;
	/// <summary>
	/// THIRD ATTACK
	/// </summary>

	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Final Attack | Damage", meta = (
			DisplayName = "Damage",
		ToolTip = "Damage del ultimo ataque" ) )
		float finalAttackDamage = 15.f;

	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Final Attack | Box", meta = (
			DisplayName = "Alcance",
			ToolTip = "Alcance del ultimo ataque" ) )
		float finalAttackReach = 300.f;
	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Final Attack | Box", meta = (
			DisplayName = "Anchura",
			ToolTip = "Anchura del ultimo ataque" ) )
		float finalAttackWidth = 120.f;
	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Final Attack | Box", meta = (
			DisplayName = "Grosor",
			ToolTip = "Grosor del ultimo ataque" ) )
		float finalAttackThickness = 60.f;
	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Final Attack | Box", meta = (
			DisplayName = "Offset",
			ToolTip = "Offset del ultimo golpe" ) )
		FVector finalAttackOffset;

	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Final Attack | Combo", meta = (
			DisplayName = "Cooldown combo finish",
			ToolTip = "Cooldown para comenzar el siguiente combo cuando el combo se finaliza en el ultimo ataque." ) )
		float finalAttackFinishCooldown = 0.9f;
	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | Final Attack | Combo", meta = (
			DisplayName = "Corta el combo?",
		ToolTip = "Si el ultimo ataque falla, se corta el combo (Inutil)" ) )
		bool finalAttackBreaksCombo = false;
	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | Final Attack | Combo", meta = (
			DisplayName = "Damage rompe el combo",
			ToolTip = "Si se recibe dano, se corta el combo y la animacion" ) )
		bool finalAttackDamageBreaksCombo = true;

	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | Final Attack | Failing Attack Penalty", meta = (
			DisplayName = "Tiene penalizacion?",
			ToolTip = "Si el ultimo ataque falla, hay penalización de tiempo" ) )
		bool finalAttackHasPenalty = false;
	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | Final Attack | Failing Attack Penalty", meta = (
			DisplayName = "Penalizacion",
			ToolTip = "Tiempo de penalizacion por fallar el ultimo ataque" ) )
		float finalAttackPenalty = 0.5f;

	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | Final Attack | Attack Forward Shift", meta = (
			DisplayName = "Distancia",
			ToolTip = "Distancia que se desplaza el jugador antes de atacar " ) )
		float finalAttackShiftDistance = 150.f;
	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | Final Attack | Attack Forward Shift", meta = (
			DisplayName = "Velocidad",
			ToolTip = "Velocidad a la que se desplaza el jugador antes de atacar, medida en unidades por segundo" ) )
		float finalAttackShiftSpeed = 750.f;

	UPROPERTY( EditAnywhere,
Category = "Design | Attack | Final Attack | Attack Forward Shift", meta = (ToolTip = "Distancia minima del enemigo para shiftear hacia el" ) )
		float finalAttackShiftMinDistance = 100.f;

	UPROPERTY( EditAnywhere,
Category = "Design | Attack | Final Attack | Attack Forward Shift", meta = (ToolTip = "Distancia maxima del enemigo para shiftear hacia el" ) )
		float finalAttackShiftMaxDistance = 300.f;

	/// <summary>
	/// AIR ATTACK
	/// </summary>
	
	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Air Attack | Damage", meta = (
			DisplayName = "Damage",
		ToolTip = "Damage del ataque aereo" ) )
		float airAttackDamage = 15.f;
	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | Air Attack | Damage", meta = (
			DisplayName = "Body Damage",
			ToolTip = "Damage causado por el jugador cayendo" ) )
		float airAttackBodyHitDamage = 15.f;

	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Air Attack | Box", meta = (
			DisplayName = "Alcance",
			ToolTip = "Alcance del ataque aereo" ) )
		float airAttackReach = 250.f;
	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Air Attack | Box", meta = (
			DisplayName = "Anchura",
			ToolTip = "Anchura del ataque aereo" ) )
		float airAttackWidth = 200.f;
	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Air Attack | Box", meta = (
			DisplayName = "Grosor",
			ToolTip = "Grosor del ataque aereo" ) )
		float airAttackThickness = 45.f;
	UPROPERTY( EditAnywhere, 
		Category = "Design | Attack | Air Attack | Box", meta = (
			DisplayName = "Offset",
			ToolTip = "Offset del golpe aereo" ) )
		FVector airAttackOffset;

	UPROPERTY( EditAnywhere,
		Category = "Design | Attack | Air Attack | Combo", meta = (
			DisplayName = "Damage rompe el combo",
			ToolTip = "Si se recibe dano, se corta el combo y la animacion" ) )
		bool airAttackDamageBreaksCombo = true;

	/// <summary>
	/// COMBO
	/// </summary>
	
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Combo", meta = (
		ToolTip = "Cooldown para resetear el combo" ) )
		float comboCooldown = .8f;



	/// <summary>
	/// OPTIONS
	/// </summary>
	
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Options", meta = (
		ToolTip = "Se puede atacar cuando el jugador está en el aire o no" ) )
		bool canAttackOnAir = true;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Options", meta = (
		ToolTip = "Se puede atacar cuando el jugador está en el aire o no" ) )
		bool canMoveWhileAttacking = true;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Options", meta = (
		ToolTip = "El combo se puede cortar mientras la habilidad de luz esta activa" ) )
		bool comboBreakWhileLightSkill = false;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Options", meta = (
		ToolTip = "El desplazamiento del ataque sigue al enemigo mas cercano" ) )
		bool playerShiftFollowsEnemy = true;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Options", meta = (
		ToolTip = "Porcentaje de modificación que puede sufrir la distancia de desplazamiento. Medido en porcentaje ( 10 => 10% )" ) )
		float playerShiftAlteration = 10.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Options", meta = (
		ToolTip = "Penalizacion cuando el jugador es golpeado y se resetea la animacion" ) )
		float penaltyWhenHitted = 0.5f;



	/// <summary>
	/// DESING ANIMATIONS
	/// </summary>

	UPROPERTY( EditAnywhere, 
		Category = "Design | Animation", meta = (
			ToolTip = "Modificación a la duración de la animación del primer ataque. Valores menores de 1 hacen que la animación se reproduzca mas lenta, valores mayores la aceleran" ) )
		float modifFirstAttackAnimation = 1.f;
	UPROPERTY( EditAnywhere, 
		Category = "Design | Animation", meta = (
			ToolTip = "Modificación a la duración de la animación del segundo ataque. Valores menores de 1 hacen que la animación se reproduzca mas lenta, valores mayores la aceleran" ) )
		float modifSecondAttackAnimation = 1.f;
	UPROPERTY( EditAnywhere, 
		Category = "Design | Animation", meta = (
			ToolTip = "Modificación a la duración de la animación del ultimo ataque. Valores menores de 1 hacen que la animación se reproduzca mas lenta, valores mayores la aceleran" ) )
		float modifFinalAttackAnimation = 1.f;
	UPROPERTY( EditAnywhere, 
		Category = "Design | Animation", meta = (
			ToolTip = "Modificación a la duración de la animación del ataque aereo. Valores menores de 1 hacen que la animación se reproduzca mas lenta, valores mayores la aceleran" ) )
		float modifAirAttackAnimation = 1.f;



	/// <summary>
	/// ART ANIMATION
	/// </summary>

	UPROPERTY( EditAnywhere, Category = "Art | Animation", meta = (
		ToolTip = "Combo Anim Montaje" ) )
		UAnimMontage* ComboAttackAnimMontage;

	UPROPERTY( EditAnywhere, Category = "Art | Animation", meta = (
		ToolTip = "Animacion del ataque aereo" ) )
		UAnimMontage* airAttackAnimMontage;	
	
	UPROPERTY( EditAnywhere, Category = "Art | VFX", meta = (
		ToolTip = "VFX del golpeo de la guadaña al colisionar con un objeto SI destructible" ) )
		UNiagaraSystem* flashHitDestructibleVFX;	
	UPROPERTY( EditAnywhere, Category = "Art | VFX", meta = (
		ToolTip = "VFX del golpeo de la guadaña al colisionar con un objeto NO destructible" ) )
		UNiagaraSystem* flashHitNotDestructibleVFX;

	/// <summary>
	/// DESTRUCTIBLES
	/// </summary>

	UPROPERTY( EditAnywhere, Category = "Destructible" )
		TSubclassOf<AActor> DestructibleClass;

	UPROPERTY( EditAnywhere, Category = "Destructible" )
		TSubclassOf<AActor> SecondDestructibleClass;
	

	/// <summary>
	/// FORCE FEEDBACK
	/// </summary>
	
	
	UPROPERTY( EditAnywhere, Category = "ForceFeedback | Effect", meta = (
		ToolTip = "Vibracion del primer ataque" ) )
		UForceFeedbackEffect* firstAttackForceFeedbackEffect;
	UPROPERTY( EditAnywhere, Category = "ForceFeedback | Effect", meta = (
		ToolTip = "Vibracion del segundo ataque" ) )
		UForceFeedbackEffect* secondAttackForceFeedbackEffect;
	UPROPERTY( EditAnywhere, Category = "ForceFeedback | Effect", meta = (
		ToolTip = "Vibracion del tercer ataque" ) )
		UForceFeedbackEffect* finalAttackForceFeedbackEffect;
	UPROPERTY( EditAnywhere, Category = "ForceFeedback | Effect", meta = (
		ToolTip = "Vibracion del ataque aereo" ) )
		UForceFeedbackEffect* airAttackForceFeedbackEffect;
	UPROPERTY( EditAnywhere, Category = "ForceFeedback | Effect", meta = (
		ToolTip = "Vibracion cuando el ataque aereo golpea el suelo" ) )
		UForceFeedbackEffect* airGroundHitForceFeedbackEffect;

public:
	// Sets default values for this component's properties
	UAttackComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void ShiftWithoutTarget(float DeltaTime, FVector& dir);

	//Colled from controller when input is recived
	void OnAttack();

	void DoAttackCombo();
	void DoAirAttack();
	void DoNextAttack();

	void MakeSingleAttack();

	void StartAttackCombo();
	void FinishAttackCombo();

	UFUNCTION( BlueprintCallable, Category = "Attack" )
		void OnAttackDamageStart();

	UFUNCTION( BlueprintCallable, Category = "Attack" )
		void OnAttackDamageEnd();

	UFUNCTION( BlueprintCallable, Category = "Attack" )
		void OnLightSkill();

	UFUNCTION( BlueprintCallable, Category = "Attack" )
		void OnAttackEnded();

	UFUNCTION( BlueprintCallable, Category = "Attack" )
		void OnMovementStops();

	UFUNCTION( BlueprintCallable, Category = "Attack" )
		void OnVerticalDrop();


	UFUNCTION()
		void ScytheBeginCollision( UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
									bool bFromSweep, const FHitResult& SweepResult );

	UFUNCTION()
		void VerticalAttackBeginCollision( UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
											UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
											bool bFromSweep, const FHitResult& SweepResult );

	void PlayComboAnimAttack();
	void PlayAirAnimAttack();
	
	void StopDrop();
	void FinishAirAttack();

	TArray<AActor*> GetDamagedEnemies();
	void HitEnemy( AActor* target );
	void BodyHitEnemy( AActor* target );
	void MakeShift();

	void CancelAttack( bool stopAnimation );

private:
	//Debug function
	FString GetStateAsName( AttackState EnumValue );

	TArray<AActor*> GetEnemies( TArray<UPrimitiveComponent*>& list );

	void SetScytheCollisionBox();

	void ApplyPenalty();
	void ApplySpecificPenalty( float penalty );

	void GetImpactPointHitToVFX( UPrimitiveComponent* OtherActor, UPrimitiveComponent* OtherComp );

	bool isADamageableEnemyComp( AActor* actor, UPrimitiveComponent* componentHit );

	bool UpdateEnemyToShitToWithClosestEnemy();
	bool isEnemyInShiftRange();

	void ShiftAndMoveToTarget(float DeltaTime );
	void ShiftAndRotateToTarget(float DeltaTime, FVector targetLocation );
};
