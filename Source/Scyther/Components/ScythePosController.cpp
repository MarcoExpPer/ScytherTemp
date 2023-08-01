/*****************************************************************************************************/
/********** Archivo fuente del componente que maneja el cambio de posicion de la guadaña *************/
/****************************************** Autor: Miguel ********************************************/
/**************************************** Fecha: 26-06-2023 ******************************************/
/*************************************** Updated: 26-06-2023 *****************************************/
/*****************************************************************************************************/


#include "ScythePosController.h"
#include "TargetingComponent.h"

// Sets default values for this component's properties
UScythePosController::UScythePosController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	actualTimeTokeep = timeTokeepScythe;

	// ...
}


// Called when the game starts
void UScythePosController::BeginPlay()
{
	Super::BeginPlay();

	character = Cast<AScytherPlayerPawn>( GetOwner() );
	if( !character )
	{
		UE_LOG( LogTemp, Error, TEXT( "ScythePosController: No se encuentra character" ) );
		return;
	}
	targetingComp = character->targetComponent;
	if( !targetingComp )
	{
		UE_LOG( LogTemp, Error, TEXT( "ScythePosController: No se encuentra el componente de targeting" ) );
		return;
	}

	scytheIsOnHand = true;
	MoveScytheToBack();
}


// Called every frame
void UScythePosController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if( scytheIsOnHand )
	{
		if( !targetingComp->actualClosestEnemy )
		{
			if( actualTimeTokeep >= 0 )
			{
				actualTimeTokeep -= DeltaTime;
			}
			else
			{
				MoveScytheToBack();
			}
		}
		else
		{
			actualTimeTokeep = timeTokeepScythe;
		}
	}
	else
	{
		if( targetingComp->actualClosestEnemy )
		{
			MoveScytheToHand();
		}
	}
}

void UScythePosController::MoveScytheToHand()
{
	if( !scytheIsOnHand )
	{
		scytheIsOnHand = true;
		FAttachmentTransformRules rules = FAttachmentTransformRules( EAttachmentRule::SnapToTarget, true );
		character->scytheMesh->AttachToComponent( character->mesh, rules, handSocket );
	}
	actualTimeTokeep = timeTokeepScythe;
}


void UScythePosController::MoveScytheToBack()
{
	if( scytheIsOnHand )
	{
		scytheIsOnHand = false;
		FAttachmentTransformRules rules = FAttachmentTransformRules( EAttachmentRule::SnapToTarget, true );
		character->scytheMesh->AttachToComponent( character->mesh, rules, backSocket );
		actualTimeTokeep = timeTokeepScythe;
	}
}

