// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BaseEnemy.h"
#include "../AIWaypointGeneral.h"
#include "NuberuPawn.generated.h"

/**
 *
 */
UCLASS()
class SCYTHER_API ANuberuPawn: public ABaseEnemy
{
	GENERATED_BODY()
public:
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	


	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Components", meta = ( DisplayName = "Nuberu", ToolTip = "Cuerpo del nuberu." ) )
		UStaticMeshComponent* nuberuBody;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Orden" )
		TArray<AAIWaypointGeneral*> TargetPoints;


	ANuberuPawn();

	virtual void Tick( float DeltaTime ) override;
	void RayAttack(FVector TargetLocation);
};
