

#include "BaseEnemyCtrl.h"
#include "../Items/ManaDrop.h"


void ABaseEnemyCtrl::BeginPlay()
{
	Super::BeginPlay();
	enemyPawn = Cast<ABaseEnemy>( GetPawn() );
}

void ABaseEnemyCtrl::rotatePawnTowardsTargetXY( FVector targetPosition )
{
	if( enemyPawn != nullptr )
	{
		//To only rotate in the XY (horizontal) plane
		targetPosition.Z = enemyPawn->GetActorLocation().Z;
		enemyPawn->SetActorRotation( ( targetPosition - enemyPawn->GetActorLocation() ).Rotation(), ETeleportType::None );
	}
}

bool ABaseEnemyCtrl::DoBasicAttack( FVector targetPosition )
{
#if WITH_EDITOR 
	UE_LOG( LogTemp, Error, TEXT( "ABaseEnemyCtlr DoAttack function is being called and is not implemented" ) );
	GEngine->AddOnScreenDebugMessage( -1, 4, FColor::Red, FString::Printf( TEXT( "ABaseEnemyCtlr DoAttack function is being called and is not implemented" ) ) );
#endif
	return false;
}
void ABaseEnemyCtrl::moveToPosition( FVector targetPosition )
{
#if WITH_EDITOR 
	UE_LOG( LogTemp, Error, TEXT( "ABaseEnemyCtlr moveTo function is being called and is not implemented" ) );
	GEngine->AddOnScreenDebugMessage( -1, 4, FColor::Red, FString::Printf( TEXT( "ABaseEnemyCtlr moveTo function is being called and is not implemented" ) ) );
#endif
}
void ABaseEnemyCtrl::hitByAttack()
{
#if WITH_EDITOR 
	UE_LOG(LogTemp, Error, TEXT("ABaseEnemyCtlr hit function is being called and is not implemented"));
	GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Red, FString::Printf(TEXT("ABaseEnemyCtlr hit function is being called and is not implemented")));
#endif
}
void ABaseEnemyCtrl::die()
{
#if WITH_EDITOR 
	UE_LOG( LogTemp, Error, TEXT( "ABaseEnemyCtlr die function is being called and is not implemented" ) );
	GEngine->AddOnScreenDebugMessage( -1, 4, FColor::Red, FString::Printf( TEXT( "ABaseEnemyCtlr die function is being called and is not implemented" ) ) );
#endif
}


