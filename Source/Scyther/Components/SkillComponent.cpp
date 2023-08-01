/*****************************************************************************************************/
/******************** Archivo fuente del componente que maneja las habilidades ***********************/
/****************************************** Autor: Alvaro ********************************************/
/**************************************** Fecha: 21-03-2023 ******************************************/
/*****************************************************************************************************/

#include "SkillComponent.h"
#include <Scyther/Interfaces/CanEnterDarkLightAreas.h>
#include "HealthComponent.h"
#include "3D_MovementComponent.h"


// Sets default values for this component's properties
USkillComponent::USkillComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


}


// Called when the game starts
void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

	character = Cast<AScytherPlayerPawn>( GetOwner() );
	if( !character )
	{
		UE_LOG( LogTemp, Error, TEXT( "Skill Component: No se encuentra character" ) );
	}

	character->SphereSkillCollision->SetSphereRadius( darkSkillRange, true );
	character->SphereSkillCollision->SetGenerateOverlapEvents( true );
	character->SphereSkillCollision->SetHiddenInGame( false );

	actualLightSkillCooldown = lightSkillCooldown;

	currentLightMana = initialLightMana;
	currentDarkMana = initialDarkMana;
}

// Called every frame
void USkillComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if( actualLightSkillCooldown > 0.f )
	{
		actualLightSkillCooldown -= DeltaTime;
	}

	if( actualLightSkillCooldown <= 0.f )
	{
		actualLightSkillCooldown = lightSkillCooldown;
		lightSkillAttacks = 0;
		actualLightSkillAttacks = 0;
	}


	if( actualLightSkillAttacks < lightSkillAttacks )
	{
		EnableLightSkill();
	}


	if( isDarkSkillAtracting )
	{
		for( int i = 0; i < enemiesAffected.Num(); i++ )
		{
			//Get the distance to move
				// Ecuation for distance = force * (distanceEnemyPlayer - safeRange) / 100
				//	- Force is a parameter that can be change by desing to ajust the force of the atraction
				//	- Multiply by the distance so to enemies far away is more powerfull but to enemies near the player is less
				//	- Safe range is another parameter to make the atraction stop near the player but not on him so the enemies doesnt hurt him
				//	- Divided by 100 because force was to strong and in this way the values of force to desing are bigger and easier to use
			float dist = ( darkSkillAtractStrenght * ( ( GetOwner()->GetActorLocation() - enemiesAffected[i]->GetActorLocation() ).Size() - darkSkillSafeRange ) ) / 100;
			dist = ( dist <= 0 ) ? 0 : dist;
			//Get the direction of the atraction
			FVector dir = GetOwner()->GetActorLocation() - enemiesAffected[i]->GetActorLocation();
			dir.Z = 0;
			dir.Normalize();
			//Check for forward collisions before movement
			FHitResult hit;
			FVector origin = enemiesAffected[i]->GetActorLocation();
			FVector end = origin + dir * dist;
			ECollisionChannel CollisionChannel = ECC_GameTraceChannel1;
			FCollisionQueryParams CollisionParams;
			CollisionParams.bTraceComplex = true;
			CollisionParams.AddIgnoredActor( GetOwner() );
			CollisionParams.AddIgnoredActor( enemiesAffected[i] );
			//If there isnt obstacles in the movement, move
			if( !GetWorld()->LineTraceSingleByChannel( hit, origin, end, CollisionChannel, CollisionParams ) )
			{
				enemiesAffected[i]->SetActorLocation( end );
			}
		}
	}

	if( isDarkSkillPushing )
	{
		for( int i = 0; i < enemiesAffected.Num(); i++ )
		{
			//GEngine->AddOnScreenDebugMessage( -1, 4, FColor::Orange, FString::Printf( TEXT( "empujando" ) ) );
			//Get the distance to move
			//Get the distance to move
				// Ecuation for distance = -(force / distanceEnemyPlayer / 100)
				//	- Force is a parameter that can be change by desing to ajust the force of the atraction
				//	- Multiply by the distance so to enemies far away is more powerfull but to enemies near the player is less
				//	- Safe range is another parameter to make the atraction stop near the player but not on him so the enemies doesnt hurt him
				//	- Divided by 100 because force was to strong and in this way the values of force to desing are bigger and easier to use
			float dist = ( -darkSkillPushStrenght / ( ( GetOwner()->GetActorLocation() - enemiesAffected[i]->GetActorLocation() ).Size() ) ) * 100;
			//dist = ( dist <= 0 ) ? 0 : dist;
			//Get the direction of the atraction
			FVector dir = GetOwner()->GetActorLocation() - enemiesAffected[i]->GetActorLocation();
			dir.Z = 0;
			dir.Normalize();
			//Check for forward collisions before movement
			FHitResult hit;
			FVector origin = enemiesAffected[i]->GetActorLocation();
			FVector end = origin + dir * dist;
			ECollisionChannel CollisionChannel = ECC_GameTraceChannel1;
			FCollisionQueryParams CollisionParams;
			CollisionParams.bTraceComplex = true;
			CollisionParams.AddIgnoredActor( GetOwner() );
			CollisionParams.AddIgnoredActor( enemiesAffected[i] );
			//If there isnt obstacles in the movement, move
			if( !GetWorld()->LineTraceSingleByChannel( hit, origin, end, CollisionChannel, CollisionParams ) )
			{
				enemiesAffected[i]->SetActorLocation( end );
			}
		}
	}
}





void USkillComponent::LightSkill()
{
	if( !character )
	{
		UE_LOG( LogTemp, Error, TEXT( "Skill Component: No se encuentra character" ) );
		return;
	}

	lightSkillAttacks++;
}

void USkillComponent::DarkSkill()
{
	if( !character )
	{
		UE_LOG( LogTemp, Error, TEXT( "Skill Component: No se encuentra character" ) );
		return;
	}
	//TODO: Traduccion de la habilidad de oscuridad
	if( skillState == SkillType::NONE )
	{
		if( currentDarkMana >= darkSkillCost || isGodMode )
		{
			//Activamos la habilidad
			EnableDarkSkill();
			//Cambiamos de modo a los enemigos que ya estan dentro del radio
			DoDarkSkill();

			simpleManaSkillCast.Broadcast( false );
		}
		else
		{
			notEnoughtManaEvent.Broadcast( false );
		}
	}
}




void USkillComponent::EnableLightSkill()
{
	if( skillState == SkillType::NONE && character->attackComponent->state == AttackState::NONE )
	{
		if( currentLightMana >= lightSkillCost || isGodMode )
		{
			//Activamos habilidad
			DoLightSkill();

			simpleManaSkillCast.Broadcast( true );
		}
		else
		{
			notEnoughtManaEvent.Broadcast( true );
		}
	}
}

void USkillComponent::EnableDarkSkill()
{
	// Change State
	skillState = SkillType::DARK;

	//Si GodMode esta desactivado
	if( !isGodMode )
	{
		UpdateMana( false, -darkSkillCost );
	}
}

void USkillComponent::DisabledSkill()
{
	//VFX
	character->niagaraSystem->SetAsset( nullptr );

	// Change State
	skillState = SkillType::NONE;
}



void USkillComponent::DoLightSkill()
{
	//Update cooldown
	actualLightSkillCooldown = lightSkillCooldown;
	
	//update light sttacks
	actualLightSkillAttacks++;

	// Change State
	skillState = SkillType::LIGHT;

	//Si GodMode esta desactivado
	if( !isGodMode )
	{
		UpdateMana( true, -lightSkillCost );
	}

	//Make a single attack
	character->attackComponent->MakeSingleAttack();

	//VFX
	character->niagaraSystem->SetAsset( lightSkillAtractionVFX );
	
}






void USkillComponent::DoDarkSkill()
{
	character->mesh->GetAnimInstance()->Montage_Play( DarkSkillAttackAnimMontage, 0.3 );
}



void USkillComponent::OnDarkSkillDamage()
{
	TArray<AActor*> overlapping;
	character->SphereSkillCollision->SetSphereRadius( darkSkillAttackRange );
	character->SphereSkillCollision->GetOverlappingActors( overlapping );

	for( int i = 0; i < overlapping.Num(); i++ )
	{
		if( overlapping[i]->GetClass()->IsChildOf( ABaseEnemy::StaticClass() ) )
		{
			if( ( (ABaseEnemy*) overlapping[i] )->isAfectedByDarkSkill )
			{
				//Hacemos daño al enemigo
				( (ABaseEnemy*) overlapping[i] )->healthComp->receiveDamage( darkSkillDamage );
			}
		}
	}
}

void USkillComponent::OnDarkSkillAtractionStart()
{
	TArray<AActor*> overlapping;
	character->SphereSkillCollision->SetSphereRadius( darkSkillRange );
	character->SphereSkillCollision->GetOverlappingActors( overlapping );

	for( int i = 0; i < overlapping.Num(); i++ )
	{
		if( overlapping[i]->GetClass()->IsChildOf( ABaseEnemy::StaticClass() ) )
		{
			if( ( (ABaseEnemy*) overlapping[i] )->isAfectedByDarkSkill )
			{
				enemiesAffected.Add( overlapping[i] );
			}
		}
	}

	isDarkSkillAtracting = true;
}

void USkillComponent::OnDarkSkillAtractionEnd()
{
	character->mesh->GetAnimInstance()->Montage_Play( DarkSkillAttackAnimMontage, 1.25 );
	character->mesh->GetAnimInstance()->Montage_JumpToSection( "Attack" );
	isDarkSkillAtracting = false;
	enemiesAffected.Empty();
}

void USkillComponent::OnDarkSkillPushStart()
{
	//character->mesh->GetAnimInstance()->Montage_Play( DarkSkillAttackAnimMontage, 0.8 );
	//character->mesh->GetAnimInstance()->Montage_JumpToSection( "Push" );

	TArray<AActor*> overlapping;
	character->SphereSkillCollision->SetSphereRadius( darkSkillRange );
	character->SphereSkillCollision->GetOverlappingActors( overlapping );

	for( int i = 0; i < overlapping.Num(); i++ )
	{
		if( overlapping[i]->GetClass()->IsChildOf( ABaseEnemy::StaticClass() ) )
		{
			if( ( (ABaseEnemy*) overlapping[i] )->isAfectedByDarkSkill )
			{
				enemiesAffected.Add( overlapping[i] );
			}
		}
	}

	isDarkSkillPushing = true;

}

void USkillComponent::OnDarkSkillPushEnd()
{
	isDarkSkillPushing = false;
	enemiesAffected.Empty();
}

void USkillComponent::OnDarkSkillEnd()
{
	DisabledSkill();
}





void USkillComponent::UpdateMana( bool isLightMana, float amount )
{
	float oldMana = currentLightMana;

	if( isLightMana )
	{
		currentLightMana += amount;

		if( currentLightMana >= maxLightMana )
		{
			currentLightMana = maxLightMana;

			if( currentLightMana == maxLightMana && currentDarkMana == maxDarkMana )
			{
				bothManaFullEvent.Broadcast();
			}
			else
			{
				manaFullEvent.Broadcast( true );
			}

		}

		manaChangedEvent.Broadcast( true, oldMana, currentLightMana );
	}
	else
	{
		currentDarkMana += amount;

		if( currentDarkMana >= maxDarkMana )
		{
			currentDarkMana = maxDarkMana;
			if( currentLightMana == maxLightMana && currentDarkMana == maxDarkMana )
			{
				bothManaFullEvent.Broadcast();
			}
			else
			{
				manaFullEvent.Broadcast( false );
			}
		}

		manaChangedEvent.Broadcast( false, currentDarkMana, maxDarkMana );
	}
}




bool USkillComponent::IsOnLightSkill()
{
	return skillState == SkillType::LIGHT;
}



