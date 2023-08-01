// Fill out your copyright notice in the Description page of Project Settings.


#include "isBooleanTrue.h"
#include "BehaviorTree/BlackboardComponent.h"


UisBooleanTrue::UisBooleanTrue( const FObjectInitializer& ObjectInitializer ): Super( ObjectInitializer )
{
	NodeName = "IsBooleanTrue";


	booleanParameter.AddBoolFilter( this, GET_MEMBER_NAME_CHECKED( UisBooleanTrue, booleanParameter ));
}

bool UisBooleanTrue::CalculateRawConditionValue( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) const
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	return BlackboardComp->GetValueAsBool( booleanParameter.SelectedKeyName );
}



FString UisBooleanTrue::GetStaticDescription() const
{
	return TEXT( "Comprueba si un booleano esta a true" );
}