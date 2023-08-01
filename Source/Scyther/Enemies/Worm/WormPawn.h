// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "fmod.hpp"
#include "FMODEvent.h"
#include "FMODBlueprintStatics.h"
#include <FMODAudioComponent.h>
#include "CoreMinimal.h"
#include "../BaseEnemy.h"
#include "../AIWaypointGeneral.h"
#include "../../Components/TargetMarkComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "WormPawn.generated.h"


UENUM( BlueprintType, meta = ( DisplayName = "Worm Type Attack" ) )
enum class AttackType: uint8
{
	VERTICAL UMETA( DisplayName = "Vertical"),
	HORIZONTAL UMETA( DisplayName = "Horizontal"),
	NOTHING UMETA( DisplayName = "Ningun Ataque")
};


UCLASS()
class SCYTHER_API AWormPawn : public ABaseEnemy
{
	GENERATED_BODY()
//Variables
public:

	AWormPawn();

	/// <summary>
	/// AUDIO Events
	/// </summary>
	
	FFMODEventInstance InstanceWraper;

	UPROPERTY( EditAnywhere, BlueprintReadWrite,Category = "Audio | FMOD", meta = ( DisplayName = "Vertical Attack Sound",
		ToolTip = "Temblor que produce el gusano cuando va a realizar \nel ataque vertical" ) )
		UFMODEvent* VerticalAttackSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio | FMOD", meta = ( DisplayName = "Horizontal Attack Sound",
		ToolTip = "Sonido que produce el gusano cuando va a realizar \nel ataque horizontal" ) )
		UFMODEvent* HorizontalAttackSound;

	/*UPROPERTY(EditAnywhere, Category = "Audio | FMOD", meta = ( DisplayName = "Roar Vertical Attack",
		ToolTip = "Rugido que realiza el gusano cuando va a hacer el ataque vertical" ) )
		UFMODEvent* RoarWormVerticalAttack;

	UPROPERTY( EditAnywhere, Category = "Audio | FMOD", meta = ( DisplayName = "Roar Horizontal Attack",
		ToolTip = "Rugido que realiza el gusano cuando va a hacer el ataque horizontal" ) )
		UFMODEvent* RoarWormHorizontalAttack;*/

	UPROPERTY( EditAnywhere, Category = "Audio | FMOD", meta = ( DisplayName = "Cracking Ground Sounds",
		ToolTip = "Sonidos de la tierra desquebrajandose cuando el gusano ataca" ) )
		UFMODEvent* CrackingWormAttack;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Orden" )
		TArray<AAIWaypointGeneral*> TargetPoints;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Components", meta = ( DisplayName = "Worm Body",
		ToolTip = "Cuerpo gusano." ) )
		USkeletalMeshComponent* bodyWorm;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Components", meta = ( DisplayName = "Worm Hitbox",
		ToolTip = "Hitbox del ataque del gusano." ) )
		UCapsuleComponent* wormCapsule;
	/*
	 *ZONAS DE VULNERABILIDAD, DESACTIVADAS
	 *
	 *UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Components", meta = ( DisplayName = "Main Damage Zone",
		ToolTip = "Zona principal vulnerable" ) )
		UBoxComponent* MainDamageZone;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Components", meta = ( DisplayName = "Head Damage Zone",
		ToolTip = "Zona cabeza vulnerable" ) )
		UBoxComponent* HeadDamageZone;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Components", meta = ( DisplayName = "Tail Damage Zone",
		ToolTip = "Zona cola vulnerable" ) )
		UBoxComponent* TailDamageZone;

		FVector ScaleHeadZoneDefault;
	FVector ScaleMainZoneDefault;
	FVector ScaleTailZoneDefault;


	*/
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Components", meta = ( DisplayName = "Floating Pawn Movement" ) )
		UFloatingPawnMovement* FloatingPawnMovement;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly )
		AttackType state = AttackType::NOTHING;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Worm Settings" )
		float PercentageVerticalLight = 50.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Worm Settings" )
		float PercentageVerticalDark = 50.f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Components" )
		UBoxComponent* EffectsPlace;
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Worm Settings" )
	float ZLocationDefault;
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Worm Settings" )
		FVector PosFinAnim;

	bool Damage = false;
	float DefaultMaxSpeed;
//Functions

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	
	virtual void Tick( float DeltaTime ) override;
	void CheckDamageArea();
	void VerticalAttack();
	void HorizontalAttack(  FVector RotationEnemy , FVector LocationToAttack);
	UFUNCTION()
		void ZonesLight();
	UFUNCTION()
		void ZonesDark();
	UFUNCTION( BlueprintCallable)
		void FinishAttack();

	UFUNCTION( BlueprintCallable )
		void MakeVulnerable();
	UFUNCTION( BlueprintCallable )
		void MakeInvulnerable();

	virtual void SetZDefault( float NewWormZ );
	void ResetAttack();

};
