#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
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

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Dash Component ", meta = ( Units = "s" ) )
		float dashDuration = 0.25f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Dash Component ", meta = ( Units = "s" ) )
		float dashCooldown = 2;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Dash Component ", meta = ( Units = "s" ) )
		float dashTimer = 0;
private:
	bool hasDashFinished = true;

	FTimerHandle DashFinishTimerHandle;
	FVector savedVelocity;

	AScytherPlayerPawn* player;

public:

	UDashComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void StartDash();
	void OnDashFinish();
};
