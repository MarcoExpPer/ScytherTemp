/*****************************************************************************************************/
/******************** Archivo fuente del componente que maneja las habilidades ***********************/
/****************************************** Autor: Alvaro ********************************************/
/**************************************** Fecha: 21-03-2023 ******************************************/
/*****************************************************************************************************/

#include "SkillComponent.h"
#include <Scyther/Interfaces/CanEnterDarkLightAreas.h>


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

	character->SphereSkillCollision->SetSphereRadius( 1000.0f, true );
	character->SphereSkillCollision->SetGenerateOverlapEvents( true );
	character->SphereSkillCollision->SetCollisionResponseToAllChannels( ECollisionResponse::ECR_Overlap );
	character->SphereSkillCollision->SetCollisionEnabled( ECollisionEnabled::NoCollision );
	character->SphereSkillCollision->SetHiddenInGame( true );
	character->SphereSkillCollision->OnComponentBeginOverlap.AddDynamic( this, &USkillComponent::BeginCollision );
	character->SphereSkillCollision->OnComponentEndOverlap.AddDynamic( this, &USkillComponent::EndCollision );

	//currentLightMana = 100.f; // TO TESTING
	//currentDarkMana = 100.f; // TO TESTING
}

// Called every frame
void USkillComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

void USkillComponent::EnabledLightSkill()
{


	if( !character )
	{
		UE_LOG( LogTemp, Error, TEXT( "Skill Component: No se encuentra character" ) );
		return;
	}

	//TODO: Traduccion de la habilidad de luz
	if( !isSkillActive )
	{
		if( (currentLightMana >= skillCost) || isGodMode )
		{

#if WITH_EDITOR 
			GEngine->AddOnScreenDebugMessage( -1, 5, FColor::Yellow, FString::Printf( TEXT( "ACTIVADA Q -> LUZ" ) ) );
#endif
			// Control de variables
			isSkillActive = true;
			
			//Si GodMode esta desactivo
			if( !isGodMode )
			{
				UpdateMana( true, -skillCost );
			}

			//Crear una funcion para la habilidad de luz
			//Habilidad de luz
			TArray<UPrimitiveComponent*> overlapedComponents;
			character->SphereSkillCollision->GetOverlappingComponents( overlapedComponents );
			for( UPrimitiveComponent* component : overlapedComponents )
			{
				if( component->GetClass() == UStaticMeshComponent::StaticClass() )
				{
					if( component->GetClass()->ImplementsInterface( UCanEnterDarkLightAreas::StaticClass() ) )
					{
						ICanEnterDarkLightAreas::Execute_SkillToLightMode( component->GetOwner() );
					}
				}
			}
			character->GetWorldTimerManager().SetTimer( SkillTimerHandle, this, &USkillComponent::DisabledSkill, skillDuration, false );

			simpleManaSkillCast.Broadcast( true );
		}
		else
		{
			notEnoughtManaEvent.Broadcast( true );
		}
	}
}

void USkillComponent::EnabledDarkSkill()
{
	if( !character )
	{
		UE_LOG( LogTemp, Error, TEXT( "Skill Component: No se encuentra character" ) );
		return;
	}
	//TODO: Traduccion de la habilidad de oscuridad
	if( !isSkillActive )
	{
		if( currentDarkMana >= skillCost || isGodMode )
		{

#if WITH_EDITOR 
			GEngine->AddOnScreenDebugMessage( -1, 5, FColor::Black, FString::Printf( TEXT( "ACTIVADA E -> OSCURIDAD" ) ) );
#endif
			// Control de las variables
			isSkillActive = true;

			//Si GodMode esta desactivo
			if( !isGodMode )
			{
				UpdateMana( false, -skillCost );
			}

			//Crear una funcion para la habilidad de oscuridad
			//Habilidad de oscuridad
			TArray<UPrimitiveComponent*> overlapedComponents;
			character->SphereSkillCollision->GetOverlappingComponents( overlapedComponents );
			for( UPrimitiveComponent* component : overlapedComponents )
			{
				if( component->GetClass() == UStaticMeshComponent::StaticClass() )
				{
					if( component->GetClass()->ImplementsInterface( UCanEnterDarkLightAreas::StaticClass() ) )
					{
						ICanEnterDarkLightAreas::Execute_SkillToDarkMode( component->GetOwner() );
					}
				}
			}
			character->GetWorldTimerManager().SetTimer( SkillTimerHandle, this, &USkillComponent::DisabledSkill, skillDuration, false );
			simpleManaSkillCast.Broadcast( false );
		}
		else
		{
			notEnoughtManaEvent.Broadcast( false );
		}
	}
}

void USkillComponent::EnabledCombinedSkill()
{
	if( (currentLightMana == maxLightMana && currentDarkMana == maxDarkMana) || isGodMode)
	{
		DoubleManaSkillCastEvent.Broadcast();
	}
}

void USkillComponent::DisabledSkill()
{
#if WITH_EDITOR 
	GEngine->AddOnScreenDebugMessage( -1, 5, FColor::Green, FString::Printf( TEXT( "FIN HABILIDAD" ) ) );
#endif
	isSkillActive = false;
}

void USkillComponent::UpdateMana( bool isLightMana, float amount )
{

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

		manaChangedEvent.Broadcast( true, currentLightMana, maxLightMana );
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

void USkillComponent::BeginCollision( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	GEngine->AddOnScreenDebugMessage( -1, 5, FColor::Yellow, TEXT( " Hace cosas la habilidad" ) );
	character->SphereSkillCollision->SetCollisionEnabled( ECollisionEnabled::QueryOnly );
	character->SphereSkillCollision->SetHiddenInGame( false );
}

void USkillComponent::EndCollision( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex )
{
	GEngine->AddOnScreenDebugMessage( -1, 5, FColor::Black, TEXT( " Hace cosas la habilidad" ) );
	character->SphereSkillCollision->SetCollisionEnabled( ECollisionEnabled::NoCollision );
}


//Crear un enumerado para light skill, dark skill y habilidad doble
