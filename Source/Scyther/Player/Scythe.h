// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Scythe.generated.h"

UCLASS()
class SCYTHER_API AScythe : public AActor
{
	GENERATED_BODY()

public:
	//Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (DisplayName = "Scythe mesh",
		ToolTip = "Malla de la scythe"))
	UStaticMeshComponent *mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (DisplayName = "Colision del filo",
		ToolTip = "Esfera de colision utilizada para el filo. Se puede aumentar su rango con la variable \"multiplicador de rango\""))
	USphereComponent *edgeCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (DisplayName = "Colision del mango",
		ToolTip = "Caja de colision del mango del arma."))
	UBoxComponent *handleCollision;

	//Parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scythe Parameters", meta = (DisplayName = "Direccion del incremento de rango",
		ToolTip = "Variable que indica en que direccion se va a incrementar el rango de la guadaña."))
	FVector rangeDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scythe Parameters", meta = (DisplayName = "Dano del mango del arma"))
	float handleDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scythe Parameters", meta = (DisplayName = "Dano del filo del arma"))
	float edgeDamage;

private:

	FVector baseEdgeCollisionLocation;
	float baseEdgeRadius;
public:	
	// Sets default values for this actor's properties
	AScythe();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Scythe Range"))
		/// <summary>
		/// Cambia el rango del filo del arma desplazando la esfera de colision y aumentando su rango en base al multiplicador.
		/// Si el multiplicador es 1, el arma esta en su rango normal.
		/// Si vale 2, tiene el doble de rango.
		/// Si vale 0.5 tiene la mitad
		/// </summary>
		/// <param name="rangeMultiplier"></param>
		void UpdateLength(float rangeMultiplier);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Toggle Scythe Collisions"))
		/// <summary>
		/// Activa o desactiva la colision de las cajs de colision del arma.
		/// </summary>
		/// <param name="newCollision"></param>
	void toggleHitBox(ECollisionEnabled::Type newCollision);
};
