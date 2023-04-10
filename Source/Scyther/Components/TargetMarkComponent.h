// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetMarkComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCYTHER_API UTargetMarkComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTargetMarkComponent();

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "MaterialBillboard", meta = ( DisplayName = "Target Mark Billboard" ) )
		class UMaterialBillboardComponent* targetMark;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Material", meta = ( DisplayName = "Posible Target Material" ) )
		class UMaterial* posibleTargetMaterial;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Material", meta = ( DisplayName = "Actual Target Material" ) )
		class UMaterial* actualTargetMaterial;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Mark", meta = ( DisplayName = "Mark Offset" ) )
		FVector markOffset = FVector(0, 0, 70);

	bool posibleTargetMarkActive = false;
	bool actualTargetMarkActive = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void TogglePosibleTargetMark( bool active );

	void ToggleActualTargetMark( bool active );
		
};
