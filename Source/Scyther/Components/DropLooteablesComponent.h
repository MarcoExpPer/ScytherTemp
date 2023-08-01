// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DropLooteablesComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCYTHER_API UDropLooteablesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDropLooteablesComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Loot | Mana", meta = (
		ToolTip = "Cantidad de dropeables de mana que spawnean cuando el enemigo muere." ) )
		float manaCrystalsAmount = 3;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Loot | Mana", meta = (
		ToolTip = "El numero de mana de luz que va a conseguir el jugador al recoger TODOS los dropeables." ) )
		float lightAmount = 15;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Loot | Mana", meta = (
		ToolTip = "El numero de mana de oscuridad que va a conseguir el jugador al recoger TODOS los dropeables." ) )
		float darkAmount = 15;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Loot | Health" )
		float healthCrystalAmount = 0;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Loot | Health")
		float healthAmount = 15;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Loot" )
		bool doUIAnimation = false;
private:
	class AScytherGameModeBase* gm;

public:
	UFUNCTION( BlueprintCallable )
	void DropLooteables( FVector location, bool isLightMana = false);

	void DropManaCrystals( FVector location, TSubclassOf<class ALootableItem>, bool isLightMana);
	void DropHealthCrystals( FVector location, TSubclassOf<class ALootableItem> );
};
