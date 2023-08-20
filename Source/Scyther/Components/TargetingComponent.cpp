// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingComponent.h"
#include <Scyther/Enemies/newWorm.h>

// Sets default values for this component's properties
UTargetingComponent::UTargetingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTargetingComponent::BeginPlay()
{
	Super::BeginPlay();

	//Recogemos la referencia al Player Pawn
	character = Cast<AScytherPlayerPawn>( GetOwner() );
	if( !character )
	{
		UE_LOG( LogTemp, Error, TEXT( "Targeting Component: No se encuentra character" ) );
	}
	// ...
	
}


// Called every frame
void UTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	

	if( character )
	{
		//Obtenemos los posibles objetivos
		posibleTargets = GetPosibleTargets( targetRange );

		//Obtenemos el mas cercano
		actualClosestEnemy = GetClosest( posibleTargets );

		//Y activamos sus marcas de fijado
		MarkTargets();
		
		//Solo si tenemos un objetivo fijado
		if( playerTarget )
		{
			//Si esta muerto, quitamos el fijado
			if( ((ABaseEnemy*) playerTarget)->GetIsDead() )
			{
				SetTarget( nullptr );
				return;
			}

			//Si se aleja mas de la distancia maxima, quitamos el fijado
			if( ( playerTarget->GetActorLocation() - GetOwner()->GetActorLocation() ).Size() > maxTargetDistance )
			{
				UTargetMarkComponent* comp = (UTargetMarkComponent*) playerTarget->GetComponentByClass( UTargetMarkComponent::StaticClass() );
				if( comp )
				{
					comp->ToggleActualTargetMark( false );
				}
				SetTarget( nullptr );
				return;
			}

			/*if( needsVision )
			{
				//Y este no es visible en este momento
				if( !IsVisible( playerTarget ) )
				{
					//Esperamos 3 ticks para quitar la marca de fijado y desfijar al objetivo
					UE_LOG( LogTemp, Warning, TEXT( "El enemigo %s no esta en vision, %i" ), *playerTarget->GetName(), targetOutOffSight );
					//Cuando se han cumplido los 3 ticks
					if( targetOutOffSight == 2 )
					{
						//Activamos la marca de posible objetivo
						targetMark = (UTargetMarkComponent*) playerTarget->GetComponentByClass( UTargetMarkComponent::StaticClass() );
						targetMark->TogglePosibleTargetMark( true );

						//Quitamos el objetivo
						SetTarget( nullptr );

						//reseteamos el conteo de ticks fuera de vision
						targetOutOffSight = 0;
					}
					targetOutOffSight++;
				}
				//Si este vuelve a ser visible despues de unos pocos ticks, reseteamos la variable contador
				else if( IsVisible( playerTarget ) && targetOutOffSight != 0 )
				{
					targetOutOffSight = 0;
				}
			}*/
		}
	}
}


/// <summary>
/// Functions that gets all posible targets in a given radious from the player
/// </summary>
/// <param name="range"> range to find the targets </param>
/// <returns> A NO-organized list of all actors in range that are a valid objetive </returns>
TArray<AActor*> UTargetingComponent::GetPosibleTargets( float range )
{
	//Every actor in radius
	TArray<AActor*> posibleTargets_ = FindActorsInRange( range );
	//Only the onles that implements CanBeZTargeted interface
	posibleTargets_ = GetJoinedEnemiesAndOthers( posibleTargets_ );
	if( needsVision )
	{
		//Remove the ones that cant be seen
		posibleTargets_ = GetVisible( posibleTargets_ );
	}
	return posibleTargets_;
}


/// <summary>
/// Function that searches among all posible targets and find the best one as the target
/// </summary>
void UTargetingComponent::FindTarget()
{
	//Reseteamos las listas
	enemiesInVisionCone = {};
	othersInVisionCone = {};
	enemiesOutVisionCone = {};
	othersOutVisionCone = {};

	//Rellenamos las listas de prioridad con los posibles actores.
	ClassifyPosibleTargets();

	//Elegimos el target
	playerTarget = ChooseTarget();

	if( playerTarget )
	{
		//Si tiene marca de fijado
		targetMark = (UTargetMarkComponent*) playerTarget->GetComponentByClass( UTargetMarkComponent::StaticClass() );
		if( targetMark )
		{
			//Activamos la marca de fijado
			targetMark->ToggleActualTargetMark( true );
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s no tiene asigando un componente Target Mark"), *playerTarget->GetName() );
		}

		// --- FMOD --- //
		// Sonido de fijar objetivo
		if( ZTargetset )
		{
			UFMODBlueprintStatics::PlayEvent2D( GetWorld(), ZTargetset, true );
		}
	}

	
	//Debug
	/*
	UE_LOG( LogTemp, Warning, TEXT( "///////////////////////////////////" ) );
	UE_LOG( LogTemp, Warning, TEXT( "Enemigos en campo de vision:" ) );
	for( int i = 0; i < enemiesInVisionCone.Num(); i++ )
	{
		UE_LOG( LogTemp, Warning, TEXT( "%s" ), *enemiesInVisionCone[i]->GetName() );
	}
	UE_LOG( LogTemp, Warning, TEXT( "Otros en campo de vision:" ) );
	for( int i = 0; i < othersInVisionCone.Num(); i++ )
	{
		UE_LOG( LogTemp, Warning, TEXT( "%s" ), *othersInVisionCone[i]->GetName() );
	}
	UE_LOG( LogTemp, Warning, TEXT( "Enemigos fuera del campo de vision:" ) );
	for( int i = 0; i < enemiesOutVisionCone.Num(); i++ )
	{
		UE_LOG( LogTemp, Warning, TEXT( "%s" ), *enemiesOutVisionCone[i]->GetName() );
	}
	UE_LOG( LogTemp, Warning, TEXT( "Otros fuera del campo de vision:" ) );
	for( int i = 0; i < othersOutVisionCone.Num(); i++ )
	{
		UE_LOG( LogTemp, Warning, TEXT( "%s" ), *othersOutVisionCone[i]->GetName() );
	}
	UE_LOG( LogTemp, Warning, TEXT( "///////////////////////////////////" ) );
	*/

}


/// <summary>
/// Function that finds all the actors in a given range from the player
/// </summary>
/// <param name="range"> Range to find the actors. Those that are further away from this range, will not be valued. </param>
/// <returns> List with all actors in range </returns>
TArray<AActor*> UTargetingComponent::FindActorsInRange( float range )
{
	TArray<AActor*> overlappingActors;
	
	//Esfera de tamaño 0 o negativo da problemas
	if( range > 0 )
	{
		//Spawneamos una esfera
		targetNet = (ATargetingNet*) GetWorld()->SpawnActor( ATargetingNet::StaticClass(), &GetOwner()->GetActorTransform() );
		
		if( targetNet )
		{
			//Asignamos el rango definido a la esfera
			targetNet->SetCollisionRadius( range );
			//Cogemos todos los actores que se encuentran en la esfera
			targetNet->GetOverlappingActors( overlappingActors );
			//Destruimos el collider
			targetNet->Destroy();
		}
		else
		{
			UE_LOG( LogTemp, Error, TEXT( "Targeting Component: No existe referencia a TargetingNet" ) );
		}
		
	}
	else
	{
		UE_LOG( LogTemp, Warning, TEXT( "El rango para el Z-Targeting está asignado a un valor de 0 o negativo. La funcion no va a encontrar objetivos." ) );
	}
	return overlappingActors;
}


/// <summary>
/// Gets a list of enemies and other actors that implements CanBeZTargeted interface joined
/// </summary>
/// <param name="list"> List to search for enemies and others </param>
/// <returns> No-organized list with enemies and others </returns>
TArray<AActor*> UTargetingComponent::GetJoinedEnemiesAndOthers( TArray<AActor*>& list )
{
	TArray<AActor*> output;
	for( int i = 0; i < list.Num(); i++ )
	{
		if( list[i]->Implements<UCanBeZtargeted>() )
		{
			output.Add( list[i] );
		}
	}
	return output;
}


/// <summary>
/// Gets a list of enemies and other actors that implements CanBeZTargeted interface separated by category
/// </summary>
/// <param name="list"> List to search for enemies and others </param>
/// <param name="outEnemies"> Output: fills that list with all enemies </param>
/// <param name="outOthers"> Output: fills that list with all other entities that can be ZTargeted </param>
void UTargetingComponent::GetSeparatedEnemiesAndOthers( TArray<AActor*>& list, TArray<AActor*>& outEnemies, TArray<AActor*>& outOthers )
{
	for( int i = 0; i < list.Num(); i++ )
	{
		if( list[i]->Implements<UCanBeZtargeted>() )
		{
			if( list[i]->GetClass()->IsChildOf(ABaseEnemy::StaticClass()) )
			{
				outEnemies.Add( list[i] );
				UE_LOG( LogTemp, Warning, TEXT( "%s" ), *list[i]->GetName() );
			}
			else
			{
				outOthers.Add( list[i] );
			}
		}
	}
}

/// <summary>
/// Gets all visible actors by the player from a list
/// </summary>
/// <param name="list"> List to search for </param>
/// <returns> List of actors that can be seen by the player </returns>
TArray<AActor*> UTargetingComponent::GetVisible( TArray<AActor*>& list )
{
	TArray<AActor*> output;
	for( int i = 0; i < list.Num(); i++ )
	{
		if( IsVisible(list[i]) )
		{
			output.Add( list[i] );
		}
	}
	return output;
}

/// <summary>
/// Checks if a given actor can be seen by the player
/// </summary>
/// <param name="target"> Actor to check </param>
/// <returns> True if the actor is visible, false otherwise </returns>
bool UTargetingComponent::IsVisible( AActor* target )
{
	int maxNumIter = 10;
	int actualNumIter = 0;
	FVector origin = GetOwner()->GetActorLocation();
	FVector end = target->GetActorLocation();
	if( target->GetClass()->IsChildOf( AWormPawn::StaticClass() ) )
	{
		end = target->GetRootComponent()->GetChildComponent( 0 )->GetComponentLocation() + FVector(0, 0, 120);
	}
	else if( target->GetClass()->IsChildOf( AnewWorm::StaticClass() ) )
	{
		end = target->GetRootComponent()->GetChildComponent( 0 )->GetComponentLocation() + FVector( 0, 0, 120 );
	}


	FHitResult hit;
	ECollisionChannel CollisionChannel = ECC_GameTraceChannel1;
	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = true;
	CollisionParams.AddIgnoredActor( GetOwner() );
	CollisionParams.AddIgnoredActor( target );
	bool endReached = false;

	while( actualNumIter++ < maxNumIter && !endReached )
	{
		//Lanzamos un rayo entre el jugador y el enemigo
		//Si no choca con nada, hay vision
		if( !GetWorld()->LineTraceSingleByChannel( hit, origin, end, CollisionChannel, CollisionParams ) )
		{
			endReached = true;
		}
		//Si choca, comprobamos el choque
		else
		{
			//Si no es una mesh, no bloquea la vision 
			if( !hit.Component->GetClass()->IsChildOf( UStaticMeshComponent::StaticClass() ) )
			{
				CollisionParams.AddIgnoredActor( hit.Actor.Get() );
				origin = hit.ImpactPoint;
			}
			//Si tiene una mesh, bloquea la vision
			else
			{
				return false;
			}
		}
	}

	return true;
}


/// <summary>
/// Function that search in posibleTargets and fill the priority lists by category.
/// </summary>
void UTargetingComponent::ClassifyPosibleTargets()
{
	for( int i = 0; i < posibleTargets.Num(); i++ )
	{
		//CALCULO DEL ANGULO ENTRE DONDE MIRA EL JUGADOR Y LA DIRECCIÖN DEL ENEMIGO
		FVector dir = ( posibleTargets[i]->GetActorLocation() - GetOwner()->GetActorLocation() );
		float dist = dir.Size();
		dir.Normalize( 1 );
		FVector playerForward = GetOwner()->GetActorForwardVector();

		FVector2D dirHoriComp = FVector2D( dir.X, dir.Y );
		FVector2D dirVertComp = FVector2D( dir.Y, dir.Z );

		FVector2D playerHoriComp = FVector2D( playerForward.X, playerForward.Y );
		FVector2D playerVertComp = FVector2D( playerForward.Y, playerForward.Z );

		//Formula del angulo entre dos vectores:  angulo = acos[ (a*b) / (|a||b|) ]

		//Angulo horizontal
		float a = UKismetMathLibrary::DotProduct2D( dirHoriComp, playerHoriComp );
		float b = dirHoriComp.Size() * playerHoriComp.Size();

		float horizontalDir = UKismetMathLibrary::DegAcos( a / b );
		//UE_LOG( LogTemp, Warning, TEXT( "%f" ), horizontalDir );

		//Angulo vertical
		float aa = UKismetMathLibrary::DotProduct2D( dirVertComp, playerVertComp );
		float bb = dirVertComp.Size() * playerVertComp.Size();

		float verticalDir = UKismetMathLibrary::Acos( aa / bb );
		//UE_LOG( LogTemp, Warning, TEXT( "%f" ), verticalDir );


		//Rellenamos las listas de prioridad
		//Comprobamos si está en el campo visual del jugador
		if( horizontalDir <= playerVisionAngle.X/2 && verticalDir <= playerVisionAngle.Y/2 )
		{
			//Comprobamos si es un enemigo
			if( posibleTargets[i]->GetClass()->IsChildOf( ABaseEnemy::StaticClass() ))
			{
				enemiesInVisionCone.Add( posibleTargets[i] );
			}
			else
			{
				othersInVisionCone.Add( posibleTargets[i] );
			}
		}
		else
		{
			//Comprobamos si es un enemigo
			if( posibleTargets[i]->GetClass()->IsChildOf( ABaseEnemy::StaticClass() ) )
			{
				enemiesOutVisionCone.Add( posibleTargets[i] );
			}
			else
			{
				othersOutVisionCone.Add( posibleTargets[i] );
			}
		}
	}
}

/// <summary>
/// Function that checks all the priority list and chooses the best target.
/// </summary>
/// <returns></returns>
AActor* UTargetingComponent::ChooseTarget()
{
	AActor* closestEnemy = nullptr;
	//Orden de prioridades:
	//Enemigos en el cono de vision		- Primero
	//Otros en el cono de vision		- Segundo
	//Enemigos fuera del cono de vision - Tercero
	//Otros fuera de cono de vision		- Cuarto
	if( enemiesInVisionCone.Num() != 0 )
	{
		closestEnemy = GetClosest( enemiesInVisionCone );
	}
	else if( othersInVisionCone.Num() != 0 )
	{
		closestEnemy = GetClosest( othersInVisionCone );
	}
	else if( !onlyTargetVisionCone )
	{
		if( enemiesOutVisionCone.Num() != 0 )
		{
			closestEnemy = GetClosest( enemiesOutVisionCone );
		}
		else if( othersOutVisionCone.Num() != 0 )
		{
			closestEnemy = GetClosest( othersOutVisionCone );
		}
	}
	else
	{
		UE_LOG( LogTemp, Warning, TEXT( "No hay enemigos que fijar" ) );
		return nullptr;
	}
	return closestEnemy;
}


/// <summary>
/// Function that finds the closest actor to the player from a given list.
/// </summary>
/// <param name="list"> Actors to check list </param>
/// <returns> Pointer to the closest enemy </returns>
AActor* UTargetingComponent::GetClosest( TArray<AActor*>& list)
{
	AActor* closestEnemy = nullptr;
	float closestDistance = targetRange * 2;

	for( AActor* enemy : list )
	{
		AWormPawn* wormPawn = Cast<AWormPawn>( enemy );
		AnewWorm* newWorm = Cast<AnewWorm>( enemy );

		if( wormPawn && wormPawn->isAttacking || newWorm && newWorm->isInAttackAnimation || ( !newWorm && !wormPawn ) )
		{
			float dist = FVector::Distance( enemy->GetActorLocation(), GetOwner()->GetActorLocation() );
			if( dist < closestDistance )
			{
				closestEnemy = enemy;
				closestDistance = dist;
			}
		}
	}

	return closestEnemy;
}

/// <summary>
/// Function that finds the closest actor to the current target
/// </summary>
/// <param name="list"> Actors to check list </param>
/// <returns> Pointer to the closest enemy </returns>
AActor* UTargetingComponent::GetClosestToTarget( TArray<AActor*>& list )
{
	AActor* closestEnemy = nullptr;
	float closestDistance = targetRange * 2;

	for( int i = 0; i < list.Num(); i++ )
	{
		//Si es el propio target, no lo usamos
		if( playerTarget->GetName() != list[i]->GetName() )
		{
			float dist = FVector::Distance( list[i]->GetActorLocation(), playerTarget->GetActorLocation() );
			if( dist < closestDistance )
			{
				closestEnemy = list[i];
				closestDistance = dist;
			}
		}
	}

	return closestEnemy;
}

/// <summary>
/// Function that recieves the player input and determines the switch of the target
/// </summary>
/// <param name="value"> Input value in the X-Axys </param>
void UTargetingComponent::SwitchTargetRight( float value )
{
	if( alreadySwitchedTarget == 0 )
	{
		if( value != 0 )
		{
			switchTargetInput.X = value;
			alreadySwitchedTarget++;
		}
	}
	else if( alreadySwitchedTarget == 1 )
	{
		switchTargetInput.X = value;
		alreadySwitchedTarget++;
	}
	else if( alreadySwitchedTarget == 2 )
	{
		SwitchTarget();
		targetSwitchedRight = true;
		alreadySwitchedTarget++;
	}
	else if( alreadySwitchedTarget == 3 )
	{
		if( targetSwitchedRight && value == 0 )
		{
			targetSwitchedRight = false;
			alreadySwitchedTarget = 0;
			switchTargetInput = FVector2D();
		}
	}
}

/// <summary>
/// Function that recieves the player input and determines the switch of the target
/// </summary>
/// <param name="value"> Input value in the X-Axys </param>
void UTargetingComponent::SwitchTargetUp( float value )
{
	if( alreadySwitchedTarget == 0 )
	{
		if( value != 0 )
		{
			switchTargetInput.Y = value;
			alreadySwitchedTarget++;
		}
	}
	else if( alreadySwitchedTarget == 1 )
	{
		switchTargetInput.Y = value;
		alreadySwitchedTarget++;
	}
	else if( alreadySwitchedTarget == 2 )
	{
		SwitchTarget();
		targetSwitchedUp = true;
		alreadySwitchedTarget++;
	}
	else if( alreadySwitchedTarget == 3 )
	{
		if( targetSwitchedUp && value == 0 )
		{
			targetSwitchedUp = false;
			alreadySwitchedTarget = 0;
			switchTargetInput = FVector2D();
		}
	}
}

/// <summary>
/// Function that changes the actual target to another one in the direction the player input requested.
/// </summary>
void UTargetingComponent::SwitchTarget()
{
	TArray<AActor*> overlappingActors;
	
	//Spawneamos una esfera
	switchTargetInput.Normalize( 0 );
	const FVector loc = playerTarget->GetActorLocation() + (FVector( switchTargetInput.X, switchTargetInput.Y, 0 ) * targetRange );
	const FTransform transform = FTransform( FRotator(0, 0, 0), loc, FVector(1, 1, 1));
	targetNet = (ATargetingNet*) GetWorld()->SpawnActor( ATargetingNet::StaticClass(), &transform );

	if( targetNet )
	{
		//Asignamos el rango definido a la esfera
		targetNet->SetCollisionRadius( targetRange );
		//Cogemos todos los actores que se encuentran en la esfera
		targetNet->GetOverlappingActors( overlappingActors );
		//Destruimos el collider
		//targetNet->Destroy();
	}
	
	overlappingActors = GetJoinedEnemiesAndOthers( overlappingActors );

	AActor* newTarget = GetClosestToTarget( overlappingActors );

	if( newTarget )
	{
		if( newTarget->GetName() == playerTarget->GetName() )
		{
			UE_LOG( LogTemp, Warning, TEXT( "El target es el mismo" ) );
		}
		else
		{
			for( int i = 0; i < posibleTargets.Num(); i++ )
			{
				if( newTarget->GetName() == posibleTargets[i]->GetName() )
				{
					//Activamos la marca de posible objetivo para el viejo target
					targetMark = (UTargetMarkComponent*) playerTarget->GetComponentByClass( UTargetMarkComponent::StaticClass() );
					if( targetMark )
					{
						targetMark->TogglePosibleTargetMark( true );
					}

					SetTarget( newTarget );

					//Activamos la marca del nuevo obetivo
					targetMark = (UTargetMarkComponent*) playerTarget->GetComponentByClass( UTargetMarkComponent::StaticClass() );
					if( targetMark )
					{
						targetMark->ToggleActualTargetMark( true );
					}

					UE_LOG( LogTemp, Warning, TEXT( "%s" ), *newTarget->GetName() );
					return;
				}
			}
			UE_LOG( LogTemp, Warning, TEXT( "No habia enemigos en esa direccion" ) );
		}
	}

}



void UTargetingComponent::MarkTargets()
{
	for( int i = 0; i < posibleTargets.Num(); i++ )
	{
		if( !playerTarget )
		{
			UTargetMarkComponent* comp = (UTargetMarkComponent*) posibleTargets[i]->GetComponentByClass( UTargetMarkComponent::StaticClass() );
			if( comp )
			{
				comp->TogglePosibleTargetMark( true );
				markedTargets.Add( posibleTargets[i] );
			}
		}
		else if( posibleTargets[i]->GetName() != playerTarget->GetName() )
		{
			UTargetMarkComponent* comp = (UTargetMarkComponent*) posibleTargets[i]->GetComponentByClass( UTargetMarkComponent::StaticClass() );
			if( comp )
			{
				comp->TogglePosibleTargetMark( true );
				markedTargets.Add( posibleTargets[i] );
			}
		}
	}

	bool removeMark = false;
	TArray<AActor*> newMarkedTargets;
	for( int i = 0; i < markedTargets.Num(); i++ )
	{
		removeMark = true;
		for( int j = 0; j < posibleTargets.Num(); j++ )
		{
			if( markedTargets[i]->GetName() == posibleTargets[j]->GetName() )
			{
				//Este elemento no necesita quitar la marca
				removeMark = false;
				newMarkedTargets.Add( markedTargets[i] );
				break;
			}
		}
		if( removeMark )
		{
			UTargetMarkComponent* comp = (UTargetMarkComponent*) markedTargets[i]->GetComponentByClass( UTargetMarkComponent::StaticClass() );
			if( comp )
			{
				comp->TogglePosibleTargetMark( false );
			}
		}
	}
	markedTargets = newMarkedTargets;
}


/// <summary>
/// Get actual target
/// </summary>
/// <returns> Actual target </returns>
AActor* UTargetingComponent::GetTarget()
{
	return playerTarget;
}

/// <summary>
/// Sets the actual target
/// </summary>
/// <param name="target"> new target </param>
void UTargetingComponent::SetTarget( AActor* target )
{
	if( !target )
	{
		// --- FMOD --- //
		// Sonido de perder el fijado

		if( ZTargetdefuse )
		{
			UFMODBlueprintStatics::PlayEvent2D( GetWorld(), ZTargetdefuse, true );
		}
	}
	else
	{
		// --- FMOD --- //
		// Sonido de fijar objetivo
		if( ZTargetset )
		{
			UFMODBlueprintStatics::PlayEvent2D( GetWorld(), ZTargetset, true );
		}
	}
	playerTarget = target;
}

TArray<AActor*> UTargetingComponent::GetTargetsByRange(float range)
{
	return GetPosibleTargets(range);
}
