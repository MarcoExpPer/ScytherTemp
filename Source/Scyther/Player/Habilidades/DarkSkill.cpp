// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkSkill.h"
#include <Scyther/Interfaces/CanEnterDarkLightAreas.h>

void ADarkSkill::onStartOverlapActor( AActor* actor )
{
	if( actor->GetClass()->ImplementsInterface( UCanEnterDarkLightAreas::StaticClass() ) )
	{
		ActorsAffected.Add( actor );
		ICanEnterDarkLightAreas::Execute_SkillToDarkMode( actor );
	}
}

void ADarkSkill::onOverlapEnterActor( AActor * actor )
{
	if( actor->GetClass()->ImplementsInterface( UCanEnterDarkLightAreas::StaticClass() ) )
	{
		ActorsAffected.Add( actor );
		ICanEnterDarkLightAreas::Execute_SkillToDarkMode( actor );
	}
}

void ADarkSkill::onOverlapExitActor( AActor * actor )
{
	if( actor->GetClass()->ImplementsInterface( UCanEnterDarkLightAreas::StaticClass() ) )
	{
		ActorsAffected.Remove( actor );
		ICanEnterDarkLightAreas::Execute_SkillEnds( actor );
	}
}

void ADarkSkill::onEndSkillActor( AActor * actor )
{
	ICanEnterDarkLightAreas::Execute_SkillEnds( actor );
}
