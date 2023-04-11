/*****************************************************************************************************/
/***************** Archivo fuente del componente que maneja el ataque del caracter *******************/
/****************************************** Autor: Alvaro ********************************************/
/****************************************** Autor: Miguel ********************************************/
/**************************************** Fecha: 24-03-2023 ******************************************/
/*****************************************************************************************************/
#pragma once

// 1- Includes
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Player/ScytherPlayerPawn.h"
#include "AttackComponent.generated.h"

UENUM( BlueprintType, meta = ( DisplayName = "Attack State" ) )
enum class AttackState: uint8
{
	FIRST UMETA( DisplayName = "Primer Ataque" ),
	SECOND UMETA( DisplayName = "Segundo Ataque" ),
	FINAL UMETA( DisplayName = "Ataque Final" ),
	AIR UMETA( DisplayName = "Ataque Aereo" ),
};

UENUM( BlueprintType, meta = ( DisplayName = "Attack Direction" ) )
enum class AttackDirection: uint8
{
	HORIZONTAL UMETA( DisplayName = "Horizontal" ),
	VERTICAL UMETA( DisplayName = "Vertical" ),
};

/// <summary>
/// </summary>
/// <seealso cref="UActorComponent" />
UCLASS()
class SCYTHER_API UAttackComponent: public UActorComponent
{
	GENERATED_BODY()

private:
	/// <value>The character< / value>
	AScytherPlayerPawn* character = nullptr;
	/// <value>Mesh which plays the animation< / value>
	USkeletalMeshComponent* meshCharacter = nullptr;
	/// <value>Targeting whitch contains all actors near of the player< / value>
	UTargetingComponent* targetComponent;
	/// <value>Mesh which cointains the scythe< / value>
	UAnimInstance* meshAnimation = nullptr;
	/// <value>  </value>
	UBoxComponent* scytheAttackCol = nullptr;
	/// <value> contains the actual state of the cooldown counter for the combo </value>
	float actualComboCooldown = 1.f;
	/// <value>  </value>
	bool isOnAttackAnim = false;
	/// <value>  </value>
	float animCooldown = 0.f;
	/// <value>  </value>
	TArray<AActor*> damagedEnemies;
	/// <value>  </value>
	FVector boxColOffset = FVector( 0, 0, 0 );


public:
	/// <summary>
	/// Return the state of the attack process, related to combo processes.
	/// </summary>
	AttackState state;
	/// <summary>
	/// Accumulator that returns the number of times the player attacks, up to a maximum of 3 times
	/// </summary>
	int attackCounter;
	/// <summary>
	/// Accumulator that counts in witch attack of the combo is the player currently in
	/// </summary>
	int actualAttackCounter;
	/// <summary>
	/// Returns true when the character is playing any attack animation at the moment.
	/// </summary>
	bool isAttacking;
	/// <summary>
	/// Returns true when the character has just performed a series of attacks that allow a combo.
	/// </summary>
	bool isComboActivate;
	/// pending
	bool scytheDoesDamage;
	/// <summary>
	/// Returns the list of enemies that have collided with the <ref>attackPlane</ref> during an attack performance.
	/// </summary>
	//TArray<AActor*> enemiesHit;


	/// <summary>
	/// FIRST ATTACK
	/// </summary>
	UPROPERTY( EditAnywhere, Category = "Design | Attack | First Attack", meta = (
		ToolTip = "Golpe del primer ataque" ) )
		float firstAttackDamage = 10.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | First Attack", meta = (
		ToolTip = "Alcance del primer ataque" ) )
		float firstAttackReach = 200.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | First Attack", meta = (
		ToolTip = "Anchura del primer ataque" ) )
		float firstAttackWidth = 150.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | First Attack", meta = (
		ToolTip = "Grosor del primer ataque" ) )
		float firstAttackThickness = 30.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | First Attack", meta = (
		ToolTip = "Duracion de activan las colisiones para golpear en el primer golpe" ) )
		float firstAttackCollisionDuration = 1.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | First Attack", meta = (
		ToolTip = "Offset del primer golpe" ) )
		FVector firstAttackOffset;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | First Attack", meta = (
		ToolTip = "Direccion del primer golpe" ) )
		AttackDirection firstAttackDirection;


	/// <summary>
	/// SECOND ATTACK
	/// </summary>
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Second Attack", meta = (
		ToolTip = "Golpe del segundo ataque" ) )
		float secondAttackDamage = 10.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Second Attack", meta = (
		ToolTip = "Alcance del segundo ataque" ) )
		float secondAttackReach = 200.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Second Attack", meta = (
		ToolTip = "Anchura del segundo ataque" ) )
		float secondAttackWidth = 150.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Second Attack", meta = (
		ToolTip = "Grosor del segundo ataque" ) )
		float secondAttackThickness = 30.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Second Attack", meta = (
		ToolTip = "Duracion de activan las colisiones para golpear en el segundo golpe" ) )
		float secondAttackCollisionDuration = 1.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Second Attack", meta = (
		ToolTip = "Offset del segundo golpe" ) )
		FVector secondAttackOffset;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Second Attack", meta = (
		ToolTip = "Direccion del segundo golpe" ) )
		AttackDirection secondAttackDirection;


	/// <summary>
	/// FINAL ATTACK
	/// </summary>
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Final Attack", meta = (
		ToolTip = "Golpe del ultimo ataque" ) )
		float finalAttackDamage = 15.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Final Attack", meta = (
		ToolTip = "Alcance del ultimo ataque" ) )
		float finalAttackReach = 250.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Final Attack", meta = (
		ToolTip = "Anchura del primer ataque" ) )
		float finalAttackWidth = 200.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Final Attack", meta = (
		ToolTip = "Grosor del ultimo ataque" ) )
		float finalAttackThickness = 45.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Final Attack", meta = (
		ToolTip = "Duracion de las colisiones para golpear en el ultimo golpe" ) )
		float finalAttackCollisionDuration = 1.5f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Final Attack", meta = (
		ToolTip = "Offset del ultimo golpe" ) )
		FVector finalAttackOffset;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Final Attack", meta = (
		ToolTip = "Direccion del ultimo golpe" ) )
		AttackDirection finalAttackDirection;


	/// <summary>
	/// AIR ATTACK
	/// </summary>
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Air Attack", meta = (
		ToolTip = "Golpe del ataque aereo" ) )
		float airAttackDamage = 15.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Air Attack", meta = (
		ToolTip = "Alcance del ataque aereo" ) )
		float airAttackReach = 250.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Air Attack", meta = (
		ToolTip = "Anchura del ataque aereo" ) )
		float airAttackWidth = 200.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Air Attack", meta = (
		ToolTip = "Grosor del ataque aereo" ) )
		float airAttackThickness = 45.f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Air Attack", meta = (
		ToolTip = "Duracion de las colisiones para golpear en el golpe aereo" ) )
		float airAttackCollisionDuration = 1.5f;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Air Attack", meta = (
		ToolTip = "Offset del golpe aereo" ) )
		FVector airAttackOffset;
	UPROPERTY( EditAnywhere, Category = "Design | Attack | Air Attack", meta = (
		ToolTip = "Direccion del golpe aereo" ) )
		AttackDirection airAttackDirection;


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


	/// <summary>
	/// DESING ANIMATIONS
	/// </summary>
	UPROPERTY( EditAnywhere, Category = "Design | Animation", meta = (
		ToolTip = "Duracion del primer ataque" ) )
		float durationFirstAttackAnimation = 1.f;
	UPROPERTY( EditAnywhere, Category = "Design | Animation", meta = (
		ToolTip = "Modificación a la duración de la animación del primer ataque. Valores menores de 1 hacen que la animación se reproduzca mas lenta, valores mayores la aceleran" ) )
		float modifFirstAttackAnimation = 1.5f;
	UPROPERTY( EditAnywhere, Category = "Design | Animation", meta = (
		ToolTip = "Duracion del segundo ataque" ) )
		float durationSecondAttackAnimation = 1.f;
	UPROPERTY( EditAnywhere, Category = "Design | Animation", meta = (
		ToolTip = "Modificación a la duración de la animación del segundo ataque. Valores menores de 1 hacen que la animación se reproduzca mas lenta, valores mayores la aceleran" ) )
		float modifSecondAttackAnimation = 1.5f;
	UPROPERTY( EditAnywhere, Category = "Design | Animation", meta = (
		ToolTip = "Duracion del ataque final del combo" ) )
		float durationFinalAttackAnimation = 1.f;
	UPROPERTY( EditAnywhere, Category = "Design | Animation", meta = (
		ToolTip = "Modificación a la duración de la animación del ultimo ataque. Valores menores de 1 hacen que la animación se reproduzca mas lenta, valores mayores la aceleran" ) )
		float modifFinalAttackAnimation = 1.5f;
	UPROPERTY( EditAnywhere, Category = "Design | Animation", meta = (
		ToolTip = "Duracion del ataque aereo" ) )
		float durationAirAttackAnimation = 1.f;
	UPROPERTY( EditAnywhere, Category = "Design | Animation", meta = (
		ToolTip = "Modificación a la duración de la animación del ataque aereo. Valores menores de 1 hacen que la animación se reproduzca mas lenta, valores mayores la aceleran" ) )
		float modifAirAttackAnimation = 1.5f;


	/// <summary>
	/// ART ANIMATION
	/// </summary>
	UPROPERTY( EditAnywhere, Category = "Art | Animation", meta = (
		ToolTip = "Animacion del primer ataque" ) )
		UAnimMontage* firstAttackAnimMontage;
	UPROPERTY( EditAnywhere, Category = "Art | Animation", meta = (
		ToolTip = "Animacion del segundo ataque" ) )
		UAnimMontage* secondAttackAnimMontage;
	UPROPERTY( EditAnywhere, Category = "Art | Animation", meta = (
		ToolTip = "Animacion del ataque final" ) )
		UAnimMontage* finalComboAttackAnimMontage;
	UPROPERTY( EditAnywhere, Category = "Art | Animation", meta = (
		ToolTip = "Animacion del ataque aereo" ) )
		UAnimMontage* airAttackAnimMontage;

public:
	// Sets default values for this component's properties
	UAttackComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void OnAttack();
	void PlayAnimAttack();
	void PlayAirAnimAttack();
	void OnFinishAttack();

	UFUNCTION( BlueprintCallable, Category = "Animation" )
		void OnAttackCombo();

	void HitEnemy();
	void DoDamage();
	void DoAirAttack();

private:
	//Debug function
	FString GetStateAsName( AttackState EnumValue );

	TArray<AActor*> GetEnemies( TArray<AActor*>& list );
};
