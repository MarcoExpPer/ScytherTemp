#pragma once

#include "fmod.hpp"
#include "FMODEvent.h"
#include "FMODBlueprintStatics.h"
#include <FMODAudioComponent.h>
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "CoreMinimal.h"
#include <Scyther/Player/ScytherPlayerController.h>
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "TargetingComponent.h"
#include "DashComponent.generated.h"

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FDashStart, bool, hasDashExecuted );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FDashEnd );

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class SCYTHER_API UDashComponent: public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY( BlueprintAssignable )
		FDashStart DashStartEvent;

	UPROPERTY( BlueprintAssignable )
		FDashEnd DashEndEvent;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Dash Component | DEBUG " )
		bool activateTextDebug = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Dash Component ", meta = ( Units = "cm" ) )
		float dashDistance = 500.f;
	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Dash Component ", meta = ( Units = "%" ) )
		float dashDistanceMultiplicator = 100;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Dash Component ", meta = ( Units = "s" ) )
		float dashDuration = 0.25f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Dash Component ", meta = ( Units = "s" ) )
		float dashInmunityDuration = 0.1f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Dash Component ", meta = ( Units = "s" ) )
		float dashCooldown = 2;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Dash Component " )
		float dashVerticalSpeed = 10;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Dash Component | idleDash" )
		bool idleDashForward = true;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Dash Component | idleDash" )
		float idleDashDistanceMultiplier = 50;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Dash Component ", meta = ( Units = "s" ) )
		float dashTimer = 0;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Dash Component ")
		bool hasDashFinished = true;

	// ----- FMOD -----

	UPROPERTY( EditAnywhere, Category = "Audio | FMOD", meta = ( DisplayName = "Dash",
		ToolTip = "Sonido que se reproduce cuando se dashea" ) )
		UFMODEvent* Dash;

	UPROPERTY( EditAnywhere, Category = "Audio | FMOD", meta = ( DisplayName = "Dash Not Available",
		ToolTip = "Sonido que se reproduce cuando no se puede dashear" ) )
		UFMODEvent* DashNotAvailable;

private:

	FTimerHandle DashFinishTimerHandle;
	FTimerHandle DashInmunityTimerHandle;
	FTimerHandle DashVFXTimerHandle;

	FVector savedVelocity;

	class AScytherPlayerPawn* player;
	class AScytherPlayerController* control;

	bool isDashAvailable = true;
	bool isDashSaved = false;
public:

	UDashComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void DashPressed();
	void StartDash();

	void SaveDashInput();
	bool CheckDashInputSaved();

	void OnDashFinish();
	void OnInmunityDashFinish();

	void StartDashVFXAndSFX();
	void OnVFXDashFinish();
	void PlayDashAvailableVFX();

};
