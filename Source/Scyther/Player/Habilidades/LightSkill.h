// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillAroundActor.h"
#include "LightSkill.generated.h"

/**
 * 
 */
UCLASS()
class SCYTHER_API ALightSkill : public ASkillAroundActor
{
	GENERATED_BODY()
	
	virtual void onStartOverlapActor( AActor* actor ) override;
	virtual void onOverlapEnterActor( AActor* actor ) override;
	virtual void onOverlapExitActor( AActor* actor ) override;
	virtual void onEndSkillActor( AActor* actor ) override;
};
