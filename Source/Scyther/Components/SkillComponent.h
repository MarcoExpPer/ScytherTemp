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
#include "../Player/ScytherPlayerPawn.h"
#include "SkillComponent.generated.h"

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FnotEnoughtMana, bool, isLightMana );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FManaFull, bool, isLightMana );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams( FManaChanged, bool, isLightMana, float, newAmount, float, maxAmount );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FSimpleManaSkillCast, bool, isLightMana );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FDoubleManaSkillCast);

//Currently theres no cooldown
//UDELEGATE( BlueprintAuthorityOnly )
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FManaSkillAvailableAgain, bool, isLightMana );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FBothManaFull);

UCLASS()
class SCYTHER_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()
	//--- Delegates ---//
	
public:
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

	bool isGodMode = false;

private:

	/// <value>The character< / value>
	AScytherPlayerPawn* character = nullptr;
	/// <value>True if the skill is already enabled</value>
	bool isSkillActive = false;
	/// <value>Controls the delay of the skill</value>
	FTimerHandle SkillTimerHandle;

public:
	/// <value>Cost of the skill</value> parametrizable
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Skill", meta = (
		ToolTip = "Coste de la habilidad" ) )
	float skillCost = 10.f;
	/// <value>Duration of the skill</value> parametrizable
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Skill", meta = (
		ToolTip = "Duracion de la habilidad" ) )
	float skillDuration = 5.f;
	// TODO: Poner tooltips a las variables
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Skill" )
		float maxLightMana = 100;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Design | Skill" )
		float maxDarkMana = 100;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Design | Skill" )
		float currentLightMana = 0;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Design | Skill" )
		float currentDarkMana = 0;
	
public:	
	// Sets default values for this component's properties
	USkillComponent();
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void EnabledLightSkill();
	void EnabledDarkSkill();
	void DisabledSkill();
	inline bool GetIsSkillActive() { return isSkillActive; };
	inline void SetIsSkillActive(const bool IsEnabled) { isSkillActive = IsEnabled; };

	UFUNCTION( BlueprintCallable)
	void UpdateMana( bool isLightMana, float amount );

	UFUNCTION( BlueprintCallable )
	void EnabledCombinedSkill();

	UFUNCTION()
		void BeginCollision( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	UFUNCTION()
		void EndCollision( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex );

protected:
	// Called when the game starts
	virtual void BeginPlay() override;	
};
