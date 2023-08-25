// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_InEnemyAlive.generated.h"
/**
 * 
 */
UCLASS()
class SCYTHER_API UEnvQueryTest_InEnemyAlive: public UEnvQueryTest
{
	GENERATED_UCLASS_BODY()

	virtual void RunTest( FEnvQueryInstance& QueryInstance ) const override;

	virtual FText GetDescriptionDetails() const override;
};
