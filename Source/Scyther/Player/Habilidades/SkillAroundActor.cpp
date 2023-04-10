// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAroundActor.h"

// Sets default values
ASkillAroundActor::ASkillAroundActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	collision = CreateDefaultSubobject<USphereComponent>( TEXT( "Collision Sphere component" ) );
	RootComponent = collision;

	collision->SetHiddenInGame( false );
}

// Called when the game starts or when spawned
void ASkillAroundActor::BeginPlay()
{
	Super::BeginPlay();

	collision->OnComponentBeginOverlap.AddDynamic( this, &ASkillAroundActor::onOverlapEnter );
	collision->OnComponentEndOverlap.AddDynamic( this, &ASkillAroundActor::onOverlapExit );

}

void ASkillAroundActor::onOverlapAtStart()
{
	TArray<UPrimitiveComponent*> overlapedComps;
	collision->GetOverlappingComponents( overlapedComps );

	for( UPrimitiveComponent* comp : overlapedComps )
	{
		UClass* c = comp->GetClass();
		
		if( comp->GetClass() == UStaticMeshComponent::StaticClass() 
			&& ActorsAffected.Find( comp->GetOwner() ) == INDEX_NONE )
		{
			onStartOverlapActor( comp->GetOwner() );
		}
	}
}
void ASkillAroundActor::onOverlapEnter( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	if( OtherComp->GetClass() == UStaticMeshComponent::StaticClass()
			&& ActorsAffected.Find( OtherActor ) == INDEX_NONE )
	{
		onOverlapEnterActor( OtherActor );
	}
}

void ASkillAroundActor::onOverlapExit( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	if( OtherComp->GetClass() == UStaticMeshComponent::StaticClass())
	{
		onOverlapExitActor( OtherActor );
	}
}

void ASkillAroundActor::endSkill()
{
	for( AActor* actor : ActorsAffected )
	{
		onEndSkillActor( actor );
	}

	Destroy();
}

/// <summary>
/// At the start of the skill, all UStaticMeshComponent overlaping the sphere will call this function with its owner (actor)
/// </summary>
/// <param name="actor"></param>
void ASkillAroundActor::onStartOverlapActor( AActor* actor )
{

}

/// <summary>
///Whenever a component overlaps enter the skill, if its a UStaticMeshComponent it will call this function with its owner (actor). 
/// </summary>
/// <param name="actor"></param>
void ASkillAroundActor::onOverlapEnterActor( AActor * actor )
{

}

/// <summary>
/// Whenever a component overlaps exits the skill, if its a UStaticMeshComponent it will call this function with its owner (actor). 
/// </summary>
/// <param name="actor"></param>
void ASkillAroundActor::onOverlapExitActor( AActor * actor )
{

}

/// <summary>
/// At the end of the skills, all actors that are inside the skill will call this function
/// </summary>
/// <param name="actor"></param>
void ASkillAroundActor::onEndSkillActor( AActor * actor )
{

}


