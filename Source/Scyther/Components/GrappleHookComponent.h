/*****************************************************************************************************/
/***************** Archivo fuente del componente que maneja el gancho del caracter *******************/
/****************************************** Autor: Alvaro ********************************************/
/**************************************** Fecha: 28-03-2023 ******************************************/
/*****************************************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Player/ScytherPlayerPawn.h"
#include "GrappleHookComponent.generated.h"

UCLASS()
class SCYTHER_API UGrappleHookComponent : public UActorComponent
{
	GENERATED_BODY()

private: 
	/// <value>The character< / value>
	AScytherPlayerPawn* character = nullptr;
	/// <value>Targeting whitch contains all actors near of the player< / value>
	UTargetingComponent* targetComponent;
	/// <value>Mesh which plays the animation< / value>
	USkeletalMeshComponent* meshCharacter = nullptr;
	/// <value>Animation that plays when the user click the button< / value>
	UAnimInstance* meshAnimation = nullptr;
	/// <value>The targe actor to stop when the character colliders</value>
	AActor* targetActor = nullptr;

public: 
	/// <summary>
	/// Return true if is throwed the hook
	/// </summary>
	bool isThrowedHook;	
	/// <summary>
	/// Return true if is throwing the hook
	/// </summary>
	UPROPERTY( EditAnywhere, Category = "Design | Animation", meta = (
	ToolTip = "Velocidad a la que se mueve el personaje con el gancho" ) )
	float speedGrapple = 0.5f;

	UPROPERTY( EditAnywhere, Category = "Art | Animation", meta = (
	ToolTip = "Animacion del lanzamiento del gancho" ) )
	UAnimMontage* throwHookAnimMontage;
public:	
	// Sets default values for this component's properties
	UGrappleHookComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ActivateGrappleHook();

	UFUNCTION( BlueprintCallable, Category = "Animation" )
	void IsFinishedAnimation();
};
