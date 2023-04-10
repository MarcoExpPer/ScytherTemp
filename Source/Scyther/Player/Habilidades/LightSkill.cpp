// Fill out your copyright notice in the Description page of Project Settings.


#include "LightSkill.h"
#include <Scyther/Interfaces/CanEnterDarkLightAreas.h>

void ALightSkill::onStartOverlapActor( AActor* actor )
{
	if( actor->GetClass()->ImplementsInterface( UCanEnterDarkLightAreas::StaticClass() ) )
	{
		ActorsAffected.Add( actor );
		ICanEnterDarkLightAreas::Execute_SkillToLightMode( actor );
	}
}

void ALightSkill::onOverlapEnterActor( AActor* actor )
{
	if( actor->GetClass()->ImplementsInterface( UCanEnterDarkLightAreas::StaticClass() ) )
	{
		ActorsAffected.Add( actor );
		ICanEnterDarkLightAreas::Execute_SkillToLightMode( actor );
	}
}

void ALightSkill::onOverlapExitActor( AActor* actor )
{
	if( actor->GetClass()->ImplementsInterface( UCanEnterDarkLightAreas::StaticClass() ) )
	{
		ActorsAffected.Remove( actor );
		ICanEnterDarkLightAreas::Execute_SkillEnds( actor );
	}
}

void ALightSkill::onEndSkillActor( AActor* actor )
{
	ICanEnterDarkLightAreas::Execute_SkillEnds( actor );
}

