// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryTest_InEnemyAlive.h"
#include <Scyther/Enemies/BaseEnemy.h>
#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"

UEnvQueryTest_InEnemyAlive::UEnvQueryTest_InEnemyAlive( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	Cost = EEnvTestCost::High;
	ValidItemType = UEnvQueryItemType_ActorBase::StaticClass();
	SetWorkOnFloatValues( false );
}

FText UEnvQueryTest_InEnemyAlive::GetDescriptionDetails() const
{
	return DescribeBoolTestParams( "Is enemy Alive" );
}


void UEnvQueryTest_InEnemyAlive::RunTest(FEnvQueryInstance& QueryInstance) const
{
	BoolValue.BindData( QueryInstance.Owner.Get(), QueryInstance.QueryID );
	bool bWantsInside = BoolValue.GetValue();

	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		AActor *a = GetItemActor(QueryInstance, It.GetIndex());
		ABaseEnemy* enemy = Cast<ABaseEnemy>(a);
		
		
		It.SetScore(TestPurpose, FilterType, !enemy->GetIsDead(), bWantsInside );
	}
}

