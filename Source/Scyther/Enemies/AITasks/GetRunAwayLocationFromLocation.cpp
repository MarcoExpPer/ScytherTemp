// Fill out your copyright notice in the Description page of Project Settings.


#include "GetRunAwayLocationFromLocation.h"

//#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIModule/Classes/AIController.h"



UGetRunAwayLocationFromLocation::UGetRunAwayLocationFromLocation()
{
	runAwayFromLocation.AddVectorFilter( this, GET_MEMBER_NAME_CHECKED( UGetRunAwayLocationFromLocation, runAwayFromLocation ) );
	calculatedLocation.AddVectorFilter( this, GET_MEMBER_NAME_CHECKED( UGetRunAwayLocationFromLocation, calculatedLocation ) );
}

EBTNodeResult::Type UGetRunAwayLocationFromLocation::ExecuteTask( UBehaviorTreeComponent& ownerComp, uint8* NodeMemory )
{
	AAIController* ctrl = Cast<AAIController>( ownerComp.GetOwner() );

	if( ctrl != nullptr )
	{
		UBlackboardComponent* bb = ownerComp.GetBlackboardComponent();
		//Calculate the direction the pawn must move to run away
		FVector runAwayLocation = bb->GetValueAsVector( runAwayFromLocation.SelectedKeyName );
		FVector pawnLocation = ctrl->GetPawn()->GetActorLocation();
		
		runAwayLocation.Z = pawnLocation.Z;
		FVector* direction = new FVector(pawnLocation - runAwayLocation);
		direction->Normalize();

		//Increase the run away position on the X axis
		if( pawnLocation.Y > runAwayLocation.Y )
		{
			direction->Y = 100;
		}
		else
		{
			direction->Y -= 100;
		}
		if( pawnLocation.X > runAwayLocation.X )
		{
			direction->X = 100;
		}
		else
		{
			direction->X -= 100;
		}

		FVector* locationToJumpTo = new FVector( pawnLocation + *direction );
		bb->SetValueAsVector( calculatedLocation.SelectedKeyName, *locationToJumpTo );


		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

FString UGetRunAwayLocationFromLocation::GetStaticDescription() const
{
	return TEXT( "Calcula la posicion destino para huir de otra posicion" );
}

