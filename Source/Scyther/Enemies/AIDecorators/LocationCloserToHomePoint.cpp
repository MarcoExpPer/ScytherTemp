// Fill out your copyright notice in the Description page of Project Settings.


#include "LocationCloserToHomePoint.h"
#include "AIModule/Classes/AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


ULocationCloserToHomePoint::ULocationCloserToHomePoint()
{
	NodeName = "LocationCloserToHomePoint";
	NotifyObserver = EBTBlackboardRestart::ResultChange;

	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;

	homeLocation.AddVectorFilter( this, GET_MEMBER_NAME_CHECKED( ULocationCloserToHomePoint, homeLocation ) );
	locationToCheck.AddVectorFilter( this, GET_MEMBER_NAME_CHECKED( ULocationCloserToHomePoint, locationToCheck ) );
	minimumDistance.AddFloatFilter( this, GET_MEMBER_NAME_CHECKED( ULocationCloserToHomePoint, minimumDistance ) );
}

bool ULocationCloserToHomePoint::CalculateRawConditionValue( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) const
{
	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();

	if(bb != nullptr ){

		FVector homeVector = bb->GetValueAsVector( homeLocation.SelectedKeyName );
		FVector toCheckVector = bb->GetValueAsVector( locationToCheck.SelectedKeyName );
		
		return ( toCheckVector - homeVector ).Size() <= bb->GetValueAsFloat( minimumDistance.SelectedKeyName );
	}
	return false;
}

FString ULocationCloserToHomePoint::GetStaticDescription() const
{
	return TEXT( "Comprueba si una localizacion esta a <= que una distancia fija del pawn" );
}

//All the following functions are necesary to make the decorator abort when locationToCheck changes its value and the conditionValue changes
void ULocationCloserToHomePoint::InitializeFromAsset( UBehaviorTree& Asset )
{

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if( BBAsset )
	{
		locationToCheck.ResolveSelectedKey( *BBAsset );
	}
	else
	{
		UE_LOG( LogBehaviorTree, Warning, TEXT( "Can't initialize %s due to missing blackboard data." ), *GetName() );
		locationToCheck.InvalidateResolvedKey();
	}
}

void ULocationCloserToHomePoint::OnBecomeRelevant( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if( BlackboardComp )
	{
		auto KeyID = locationToCheck.GetSelectedKeyID();
		BlackboardComp->RegisterObserver( KeyID, this, FOnBlackboardChangeNotification::CreateUObject( this, &ULocationCloserToHomePoint::OnBlackboardKeyValueChange ) );
	}
}

void ULocationCloserToHomePoint::OnCeaseRelevant( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if( BlackboardComp )
	{
		BlackboardComp->UnregisterObserversFrom( this );
	}
}

EBlackboardNotificationResult ULocationCloserToHomePoint::OnBlackboardKeyValueChange( const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID )
{
	UBehaviorTreeComponent* BehaviorComp = (UBehaviorTreeComponent*) Blackboard.GetBrainComponent();
	if( BehaviorComp == nullptr )
	{
		return EBlackboardNotificationResult::RemoveObserver;
	}

	if( locationToCheck.GetSelectedKeyID() == ChangedKeyID )
	{
		BehaviorComp->RequestExecution( this );
	}
	return EBlackboardNotificationResult::ContinueObserving;
}