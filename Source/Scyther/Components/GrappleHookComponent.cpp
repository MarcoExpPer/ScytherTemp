/*****************************************************************************************************/
/***************** Archivo fuente del componente que maneja el gancho del caracter *******************/
/****************************************** Autor: Alvaro ********************************************/
/**************************************** Fecha: 28-03-2023 ******************************************/
/*****************************************************************************************************/

#include "GrappleHookComponent.h"
#include "TargetingComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include <DrawDebugHelpers.h>

// Sets default values for this component's properties
UGrappleHookComponent::UGrappleHookComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrappleHookComponent::BeginPlay()
{
	Super::BeginPlay();

	character = Cast<AScytherPlayerPawn>( GetOwner() );
	// TODO: Agregar cast de la guadaña y comprobar si la tiene el player
	if( !character )
	{
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra character" ) );
		return;
	}
	targetComponent = character->targetComponent;
	if( !targetComponent )
	{
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra componente target" ) );
		return;
	}
	meshCharacter = character->mesh;
	if( !meshCharacter )
	{
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra mesh del character" ) );
		return;
	}
	meshAnimation = meshCharacter->GetAnimInstance();
}


// Called every frame
void UGrappleHookComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Makes the movement of the character when finish the animation
	if( isThrowedHook )
	{
		// TODO: Por ahora solo es horizontal
		FVector actualPosition = character->GetActorLocation();
		FVector newPosition = FVector( actualPosition.X, actualPosition.Y * speedGrapple * DeltaTime, actualPosition.Z );
		character->SetActorLocation( newPosition );
		// TODO: Tendria que parar
		if( character->GetActorLocation().Equals( targetActor->GetActorLocation() ) )
		{
			isThrowedHook = false;
			// Habra que hacerlo mejor con el estado del personaje
			//meshAnimation->Montage_Pause( throwHookAnimMontage );
		}
		
	}
}

void UGrappleHookComponent::ActivateGrappleHook()
{
	targetActor = targetComponent->playerTarget;
	if( targetActor )
	{
		if( targetActor->GetRootComponent()->ComponentHasTag( FName("Grappleable") ) )
		{
#if WITH_EDITOR 
			GEngine->AddOnScreenDebugMessage( -1, 5, FColor::Purple, FString::Printf( TEXT( "Tengo objeto targeteado" ) ) );
#endif
			if( !meshAnimation || !throwHookAnimMontage )
			{
				UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra la animacion de la mesh o la animacion a ejecutar" ) );
				return;
			}
			meshAnimation->Montage_Play( throwHookAnimMontage );
		}
	}
}

void UGrappleHookComponent::IsFinishedAnimation()
{
	meshAnimation->Montage_Pause( throwHookAnimMontage );
	UE_LOG( LogTemp, Error, TEXT( "Grapple Component: Ha acabado la animacion y el caracter se mueve" ) );
	isThrowedHook = true;
}

