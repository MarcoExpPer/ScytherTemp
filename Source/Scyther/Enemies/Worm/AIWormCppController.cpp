// Fill out your copyright notice in the Description page of Project Settings.


#include "AIWormCppController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../AIWaypointGeneral.h"
//#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "../BaseEnemy.h"
//#include "NavigationSystem.h"



void AAIWormCppController::BeginPlay()
{
	Super::BeginPlay();
	worm = Cast<AWormPawn>( this->GetPawn() );
	UBlackboardComponent* pBlackBoardComponent = BrainComponent->GetBlackboardComponent();
	pBlackBoardComponent->SetValueAsBool( "isMoving?", true );
}
void AAIWormCppController::UpdateNextTargetPoint()
{
	//
	TArray<AAIWaypointGeneral*> aLista = wormPawn->TargetPoints;

	//Se obtiene la referencia que va desde el Controlador de IA al BlackBoardComponent
	UBlackboardComponent* pBlackBoardComponent = BrainComponent->GetBlackboardComponent();

	//Guarda en TargetPointOrder el valor que tiene el BlackBoard en el KEY TargetPointOrder
	//Este número nos indica el orden en el que el moñeco se va a mover por el nivel
	int32 iTargetPointOrder = pBlackBoardComponent->GetValueAsInt( "TargetPointOrder" );

	//Pilla el número del TargetPoints que hay en el nivel y cuando lo supera se reinicia al primero

	if(iTargetPointOrder >= aLista.Num())
	{
		//Pone el valor de iTargetPointOrder a 0
		iTargetPointOrder = 0;
		pBlackBoardComponent->SetValueAsInt( "TargetPointOrder", iTargetPointOrder );
	}

	//Setteamos el TargetPointPosition con la posicion de ese TargetPoint en el nivel y detenemos el ciclo con el break;
	pBlackBoardComponent->SetValueAsVector( "TargetPointPosition", aLista[iTargetPointOrder]->GetActorLocation() );


	// Por último, incrementamso el valor de TargetPointOrder del Blackboard
	pBlackBoardComponent->SetValueAsInt( "TargetPointOrder", ( iTargetPointOrder + 1 ) );


}

float AAIWormCppController::GetTimeDelay()
{
	return TimeDelayToAttack;
}


float AAIWormCppController::GetVerticalLight()
{
	return PercentageVerticalLight;
}

float AAIWormCppController::GetVerticalDark()
{
	return PercentageVerticalDark;
}



void AAIWormCppController::CheckNearByWorm()
{
	//Cogemos la referencia al pawn del gusano
	APawn* pWormPawn = GetPawn();

	//guardamos la posicion del gusano
	FVector MultiSphereStart = pWormPawn->GetActorLocation();

	//se crea un vector nuevo a partir de la posicion del gusano
	FVector MultiSphereEnd = MultiSphereStart + FVector( 0, 0, 15 );

	//array tipos de objetos para el sphereTrace
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add( UEngineTypes::ConvertToObjectType( ECC_Pawn ) );

	//array de los actores a ignorar por el metodo
	//IMPORTANTE!!!! mas tarde quizas hay que hacer una pull de enemigos porque quizas puede perseguir a otros enemigos y no se si es lo deseado
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add( pWormPawn );

	//array con los hits que detecte la esfera
	TArray<FHitResult> OutHits;


	//esfera que detecta al player
	bool bResult = UKismetSystemLibrary::SphereTraceMultiForObjects( GetWorld(),
		MultiSphereStart,
		MultiSphereEnd,
		RangeHorizontalAttack,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		OutHits,
		true );

	UBlackboardComponent* BlackboardComponent = BrainComponent->GetBlackboardComponent();


	//si hay resultados en la esfera
	if( bResult )
	{
		//recorre todos los objetos de outhits
		for( int32 i = 0; i < OutHits.Num(); i++ )
		{
			FHitResult Hit = OutHits[i];

			//referencia al personaje
			APawn* pPawn = UGameplayStatics::GetPlayerPawn( GetWorld(), 0 );

			//si el actor detectado es el personaje, entonces seteamos la key TargetActorToFollow en la blackboard con la referencia del personaje
			if( Hit.GetActor() == pPawn )
			{
				BlackboardComponent->SetValueAsObject( "SelfActor", pPawn );
				break;

			}

		}
	}
	else
	{
		BlackboardComponent->SetValueAsObject( "TargetActorToFollow", NULL );
	}
}

EPathFollowingRequestResult::Type AAIWormCppController::MoveToEnemy()
{

	UBlackboardComponent* BlackboardComponent = BrainComponent->GetBlackboardComponent();


	//obtenemos la referencia al actor que esta guardado en la blackboard en el key TargetActorToFollow
	AActor* CharacterActor = Cast<AActor>( BlackboardComponent->GetValueAsObject( "TargetActorToFollow" ) );


	//se inicia el proceso de mover al personaje
	EPathFollowingRequestResult::Type MoveToActorResult = MoveToActor( CharacterActor );
	
	return MoveToActorResult;
	
}

void AAIWormCppController::moveToPosition( FVector targetPosition )
{
	if( worm != nullptr )
	{
		this->MoveToLocation( targetPosition, 1.0f, false, true);
	}
}

bool AAIWormCppController::DoBasicAttack( FVector targetPosition )
{
	UBlackboardComponent* BlackboardComponent = BrainComponent->GetBlackboardComponent();
	if( worm != nullptr )
	{
		bool AttackType = BlackboardComponent->GetValueAsBool( "AttackType" );
		if(AttackType == true)
		{//vertical
			StopMovement();
			worm->VerticalAttack();
			return true;
		}
		else
		{//horizontal
			FVector playerPosition = BlackboardComponent->GetValueAsVector( "PlayerPosition" );

			if( (abs(playerPosition.X - targetPosition.X) >50.f) && (abs(playerPosition.Y - targetPosition.Y )>50.f)  )
			{
				StopMovement();
				FVector PositionTeleport = FVector( playerPosition.X + targetPosition.X, playerPosition.Y + targetPosition.Y, targetPosition.Z );
				FHitResult CollisionTeleport;
				worm->K2_SetActorLocation( PositionTeleport, false, CollisionTeleport, true );
				worm->HorizontalAttack( targetPosition );
				return true;
			}
			else
			{
				StopMovement();
				worm->VerticalAttack();
				return true;
			}
		}
	}

	return false;
}
