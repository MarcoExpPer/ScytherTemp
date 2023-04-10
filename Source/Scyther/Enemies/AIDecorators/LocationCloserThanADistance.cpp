// Fill out your copyright notice in the Description page of Project Settings.


#include "LocationCloserThanADistance.h"	
#include "AIModule/Classes/AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


ULocationCloserThanADistance::ULocationCloserThanADistance()
{
	NodeName = "LocationCloserThanADistance";
	NotifyObserver = EBTBlackboardRestart::ResultChange;

	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;

	locationToCheck.AddVectorFilter( this, GET_MEMBER_NAME_CHECKED( ULocationCloserThanADistance, locationToCheck ) );
	minimumDistance.AddFloatFilter( this, GET_MEMBER_NAME_CHECKED( ULocationCloserThanADistance, minimumDistance ) );
}

bool ULocationCloserThanADistance::CalculateRawConditionValue( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) const
{
	AAIController *ctrl = Cast<AAIController>( OwnerComp.GetOwner() );

	if( ctrl != nullptr )
	{
		UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();

		FVector location = bb->GetValueAsVector( locationToCheck.SelectedKeyName );
		FVector pawnLocation = ctrl->GetPawn()->GetActorLocation();

		return ( location - pawnLocation ).Size() <= bb->GetValueAsFloat(minimumDistance.SelectedKeyName);
	}
	return false;
}

FString ULocationCloserThanADistance::GetStaticDescription() const
{
	return TEXT( "Comprueba si una localizacion esta a <= que una distancia fija del pawn" );
}

//All the following functions are necesary to make the decorator abort when locationToCheck changes its value and the conditionValue changes
void ULocationCloserThanADistance::InitializeFromAsset( UBehaviorTree& Asset )
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

void ULocationCloserThanADistance::OnBecomeRelevant( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if( BlackboardComp )
	{
		auto KeyID = locationToCheck.GetSelectedKeyID();
		BlackboardComp->RegisterObserver( KeyID, this, FOnBlackboardChangeNotification::CreateUObject( this, &ULocationCloserThanADistance::OnBlackboardKeyValueChange ) );
	}
}

void ULocationCloserThanADistance::OnCeaseRelevant( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if( BlackboardComp )
	{
		BlackboardComp->UnregisterObserversFrom( this );
	}
}

EBlackboardNotificationResult ULocationCloserThanADistance::OnBlackboardKeyValueChange( const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID )
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