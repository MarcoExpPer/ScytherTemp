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
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "WormPawn.generated.h"


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

	UPROPERTY( EditAnywhere, BlueprintReadWrite,Category = "Audio | FMOD", meta = ( DisplayName = "Sound Shake of The Worm",
		ToolTip = "Temblor que produce el gusano cuando va a atacar" ) )
		UFMODEvent* EventWormShake;

	UPROPERTY( EditAnywhere, Category = "Audio | FMOD", meta = ( DisplayName = "Roar Vertical Attack",
		ToolTip = "Rugido que realiza el gusano cuando va a hacer el ataque vertical" ) )
		UFMODEvent* RoarWormVerticalAttack;

	UPROPERTY( EditAnywhere, Category = "Audio | FMOD", meta = ( DisplayName = "Roar Horizontal Attack",
		ToolTip = "Rugido que realiza el gusano cuando va a hacer el ataque horizontal" ) )
		UFMODEvent* RoarWormHorizontalAttack;

	UPROPERTY( EditAnywhere, Category = "Audio | FMOD", meta = ( DisplayName = "Cracking Ground Sounds",
		ToolTip = "Sonidos de la tierra desquebrajandose cuando el gusano ataca" ) )
		UFMODEvent* CrackingWormAttack;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Orden" )
		TArray<AAIWaypointGeneral*> TargetPoints;


	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Components", meta = ( DisplayName = "worm Body",
		ToolTip = "Cuerpo de ayuda del gusano." ) )
		UStaticMeshComponent* wormBody;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Components", meta = ( DisplayName = "real worm Body",
		ToolTip = "Cuerpo del gusano." ) )
		UStaticMeshComponent* wormBodyReal;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Components", meta = ( DisplayName = "Worm Hitbox",
		ToolTip = "Hitbox del ataque del gusano." ) )
		UCapsuleComponent* wormAttackCapsule;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Components", meta = ( DisplayName = "Main Damage Zone",
		ToolTip = "Zona principal de zona vulnerable" ) )
		UBoxComponent* MainDamageZone;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Components", meta = ( DisplayName = "Target Mark Component" ) )
		UTargetMarkComponent* targetMarkComp;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Components", meta = ( DisplayName = "Worm Main Vulnerability",
		ToolTip = "Hitbox de debilidad principal." ) )
		UBoxComponent* MainVulnerabilityZone;

	bool Damage = false;
//Functions
public:
	
	virtual void Tick( float DeltaTime ) override;
	void CheckDamageArea();
	void VerticalAttack();
	void HorizontalAttack(  FVector EnemyPosition );
	void ResetAttack();

};
