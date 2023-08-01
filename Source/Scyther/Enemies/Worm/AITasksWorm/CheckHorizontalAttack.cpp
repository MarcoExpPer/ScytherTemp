// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckHorizontalAttack.h"

#include "../../Worm/AIWormCppController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Scyther/Components/3D_MovementComponent.h"
#include "Scyther/Enemies/Worm/WormPawn.h"
#include "Scyther/Player/ScytherPlayerPawn.h"

UCheckHorizontalAttack::UCheckHorizontalAttack()
{
	locationToAttack.AddVectorFilter( this, GET_MEMBER_NAME_CHECKED( UCheckHorizontalAttack, locationToAttack ) );
}

EBTNodeResult::Type UCheckHorizontalAttack::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	bNotifyTick = 1;
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	targetPosition = BlackboardComponent->GetValueAsVector( locationToAttack.SelectedKeyName );
	ObjectTypes.Add( UEngineTypes::ConvertToObjectType( ECC_WorldStatic ) );
	playerPawn = UGameplayStatics::GetPlayerPawn( GetWorld(), 0 );
	RangeHorizontalAttack = Cast<AAIWormCppController>( OwnerComp.GetOwner() )->RangeHorizontalAttack;
	//ObjectTypes.Add( UEngineTypes::ConvertToObjectType( ECC_Pawn ) );
	return EBTNodeResult::InProgress;
}

void UCheckHorizontalAttack::TickTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds )
{
	
	if( Cast<AScytherPlayerPawn>( playerPawn )->state == MovementState::WALKING )
	{
 		UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
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
		TArray<AActor*> ActorsToIgnore;

		bool bResult1 = UKismetSystemLibrary::SphereTraceMultiForObjects( GetWorld(),
			playerPosition,
			FVector( TeleportX, TeleportY, playerPosition.Z ),
			8.f,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForDuration,
			OutHits,
			true );
		/*bool bResult2 = UKismetSystemLibrary::SphereTraceMultiForObjects( GetWorld(),
			playerPosition,
			FVector( ( targetPosition.X - TeleportRandX ), ( targetPosition.Y - TeleportRandY ), playerPosition.Z ),
			80.f,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForDuration,
			OutHits,
			true );*/

		if( bResult1 == false /*&& bResult2 == false */ )
		{
			ABaseEnemyCtrl* ctrl = Cast<ABaseEnemyCtrl>( OwnerComp.GetOwner() );
			APawn* enemy = ctrl->GetPawn();
			float ZDefault = Cast<AWormPawn>( enemy )->ZLocationDefault;
			FVector PosFin = FVector( ( targetPosition.X - TeleportRandX ), ( targetPosition.Y - TeleportRandY ), ZDefault );
			FVector PosIni = FVector( TeleportX, TeleportY, ZDefault );
			FVector RotationAttack = ( PosFin - PosIni );
			Cast<AWormPawn>( enemy )->PosFinAnim = FVector( ( targetPosition.X - TeleportRandX ), ( targetPosition.Y - TeleportRandY ), playerPosition.Z );
			//FVector VectorImpulse = FVector( TeleportRandX, TeleportRandY, TeleportZ );
			BlackboardComponent->SetValueAsVector( "PositionHorizontalAttack", PosIni );
			BlackboardComponent->SetValueAsVector( "RotationAttack", RotationAttack );
			BlackboardComponent->SetValueAsBool( "IsCheck?", true );
			BlackboardComponent->SetValueAsBool( "DontCheck", true );

			cont = 0;

			UE_LOG( LogTemp, Warning, TEXT( "NO HAY OBSTACULO" ) );
			//modificar a la nueva velocidad del gusano para ir al punto desde donde se lanza
			Cast<AWormPawn>( enemy )->FloatingPawnMovement->MaxSpeed = Cast<AAIWormCppController>(ctrl)->SpeedMovementHorizontalAttack;
			FinishLatentTask( OwnerComp, EBTNodeResult::Succeeded );

		}
		else if( cont >= 100 )
		{

			BlackboardComponent->SetValueAsBool( "AttackType", true );
			BlackboardComponent->SetValueAsBool( "IsCheck?", true );
			//BlackboardComponent->SetValueAsObject("ShakeWorm", Nombre del audio para vertical);
			cont = 0;
			UE_LOG( LogTemp, Warning, TEXT( "HAY OBSTACULO" ) );
			FinishLatentTask( OwnerComp, EBTNodeResult::Succeeded );
		}
		else
		{
			cont++;
		}
	}
}

FString UCheckHorizontalAttack::GetStaticDescription() const
{
	return TEXT( "Realiza el ataque horizontal desde una posicion aleatorio en un rango" );
}
