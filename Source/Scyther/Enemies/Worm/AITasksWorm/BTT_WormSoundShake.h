// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "fmod.hpp"
#include "FMODEvent.h"
#include "FMODBlueprintStatics.h"
#include <FMODAudioComponent.h>
#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_WormSoundShake.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API UBTT_WormSoundShake : public UBTTaskNode
{
	GENERATED_BODY()

	UBTT_WormSoundShake();

	//Se llama al iniciar este Task, tiene que retomar Suceeded, Failed o inProgress
	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory );

	//Permite definir una descripción para este Task. Este texto se ve en ele nodo al agregarlo al B.T
	virtual FString GetStaticDescription() const override;
	
	UPROPERTY( EditAnywhere, Category = Blackboard )
		FBlackboardKeySelector SoundShake;

};
