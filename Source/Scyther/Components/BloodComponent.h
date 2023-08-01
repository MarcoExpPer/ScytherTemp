// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BloodComponent.generated.h"

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FBloodChangedEvent, float, oldBlood, float, newBlood );

UDELEGATE( BlueprintAuthorityOnly )
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FBloodUsedEvent, float, currentBlood );


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCYTHER_API UBloodComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "BloodComponent | Blood", meta = ( DisplayName = "current blood" ) )
		float currentBlood;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "BloodComponent | Blood", meta = ( DisplayName = "Initial blood") )
		float initialBlood = 30;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "BloodComponent | Blood", meta = ( DisplayName = "Max blood" ) )
		float maxBlood = 60;

	bool isInGodMode = false;
	
	UPROPERTY( BlueprintAssignable, Category = "BloodComponent | Events" )
		FBloodChangedEvent bloodChangedEvent;

	UPROPERTY( BlueprintAssignable, Category = "BloodComponent | Events" )
		FBloodUsedEvent bloodUsedEvent;

	UBloodComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION( BlueprintCallable, Category = "BloodComponent", meta = ( DisplayName = "Change blood" ) )
		void changeBlood( float change, bool sendEvent = true );

	UFUNCTION( BlueprintCallable, Category = "BloodComponent", meta = ( DisplayName = "Use blood" ) )
		float useBlood( float bloodToUse, bool sendEvent = true );
		


	/// <summary>
	/// Use it to activate this component god mode. Making the user inmune to dasheable and inevitable attacks
	/// </summary>
	void becomeGod();
	/// <summary>
	/// Use it to remove this component god mode. Making the user vulnerable to dasheable and inevitable attacks
	/// </summary>
	void becomeMortal();
};
