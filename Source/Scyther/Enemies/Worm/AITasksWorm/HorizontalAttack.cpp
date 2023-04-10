// Fill out your copyright notice in the Description page of Project Settings.


#include "HorizontalAttack.h"
#include "../../../Player/ScytherPlayerPawn.h"
#include "../../Worm/AIWormCppController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Scyther/Enemies/Worm/WormPawn.h"

UHorizontalAttack::UHorizontalAttack()
{
	locationToAttack.AddVectorFilter( this, GET_MEMBER_NAME_CHECKED( UHorizontalAttack, locationToAttack ) );
}

EBTNodeResult::Type UHorizontalAttack::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	bNotifyTick = 1;
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	targetPosition = BlackboardComponent->GetValueAsVector( locationToAttack.SelectedKeyName );
	ObjectTypes.Add( UEngineTypes::ConvertToObjectType( ECC_WorldStatic ) );
	//ObjectTypes.Add( UEngineTypes::ConvertToObjectType( ECC_Pawn ) );
	return EBTNodeResult::InProgress;
}

void UHorizontalAttack::TickTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds )
{

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	
	AWormPawn* enemy = Cast<AWormPawn>( OwnerComp.GetOwner() );
	//AScytherPlayerPawn* player = Cast<AScytherPlayerPawn>( OwnerComp.GetOwner() );
	FVector EnemyPosition = BlackboardComponent->GetValueAsVector( "EnemyPosition" );
	FVector playerPosition = BlackboardComponent->GetValueAsVector( "PlayerPosition" );

	TeleportRandX = FMath::RandRange( 0.0f, RangeHorizontalAttack );
	TeleportRandY = RangeHorizontalAttack - TeleportRandX;

	if( FMath::RandRange( 0.0f, 1.0f ) <= 0.5f )
	{
		TeleportRandX = TeleportRandX * -1.0f;
	}
	if( FMath::RandRange( 0.0f, 1.0f ) <= 0.5f )
	{
		TeleportRandY = TeleportRandY * -1.0f;
	}
	TeleportX = targetPosition.X + TeleportRandX;
	TeleportY = targetPosition.Y + TeleportRandY;
	TeleportZ = EnemyPosition.Z;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add( enemy );
	//ActorsToIgnore.Add( player );
	
	bool bResult1 = UKismetSystemLibrary::SphereTraceMultiForObjects( GetWorld(),
		playerPosition,
		FVector( TeleportX, TeleportY, playerPosition.Z ),
		80.f,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		OutHits,
		true );
	bool bResult2 = UKismetSystemLibrary::SphereTraceMultiForObjects( GetWorld(),
		playerPosition,
		FVector( ( targetPosition.X - TeleportRandX ), ( targetPosition.Y - TeleportRandY ), playerPosition.Z ),
		80.f,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		OutHits,
		true );

	if( bResult1 == false && bResult2 == false && valid == false )
	{
		valid = true;
		FVector VectorImpulse;
		if( result == false )
		{
			VectorImpulse = FVector( TeleportRandX, TeleportRandY, TeleportZ );
			ABaseEnemyCtrl* ctrl = Cast<ABaseEnemyCtrl>( OwnerComp.GetOwner() );
			result = ctrl->DoBasicAttack( VectorImpulse );
		}
		if( result == true )
		{
			result = false;
			cont = 0;
			valid = false;
			UE_LOG( LogTemp, Warning, TEXT( "NO HAY OBSTACULO" ) );
			VectorImpulse = FVector( 0, 0, 0 );
			FinishLatentTask( OwnerComp, EBTNodeResult::Succeeded );
		}
		else
		{
			valid = false;
		}
	}
	else if( cont >= 100 )
	{
		ABaseEnemyCtrl* ctrl = Cast<ABaseEnemyCtrl>( OwnerComp.GetOwner() );
		BlackboardComponent->SetValueAsBool( "AttackType", true );
		ctrl->DoBasicAttack( targetPosition );
		result = false;
		cont = 0;
		valid = false;
		UE_LOG( LogTemp, Warning, TEXT( "HAY OBSTACULO" ) );
		FinishLatentTask( OwnerComp, EBTNodeResult::Succeeded );
	}
	else
	{
		cont++;
	}
}

FString UHorizontalAttack::GetStaticDescription() const
{
	return TEXT("Realiza el ataque horizontal desde una posicion aleatorio en un rango");
}
