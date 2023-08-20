

#include "BaseEnemyCtrl.h"
#include <Scyther/ScytherGameModeBase.h>
#include <Scyther/CombatManager.h>
#include <Scyther/Components/HealthComponent.h>
#include <Kismet/GameplayStatics.h>

void ABaseEnemyCtrl::BeginPlay()
{
	Super::BeginPlay();
	enemyPawn = Cast<ABaseEnemy>( GetPawn() );

	gm = Cast<AScytherGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	enemyPawn->healthComp->zerohealthEvent.AddDynamic( this, &ABaseEnemyCtrl::whenHpGoesTo0 );
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

void ABaseEnemyCtrl::combatStateChanged(){}

void ABaseEnemyCtrl::makeNextAttack()
{

}

bool ABaseEnemyCtrl::DoBasicAttack( FVector targetPosition )
{
#if WITH_EDITOR 
	UE_LOG( LogTemp, Error, TEXT( "ABaseEnemyCtlr DoAttack function is being called and is not implemented" ) );
	GEngine->AddOnScreenDebugMessage( -1, 4, FColor::Red, FString::Printf( TEXT( "ABaseEnemyCtlr DoAttack function is being called and is not implemented" ) ) );
#endif
	return false;
}
EPathFollowingRequestResult::Type ABaseEnemyCtrl::moveToPosition( FVector targetPosition )
{
#if WITH_EDITOR 
	UE_LOG( LogTemp, Error, TEXT( "ABaseEnemyCtlr moveTo function is being called and is not implemented" ) );
	GEngine->AddOnScreenDebugMessage( -1, 4, FColor::Red, FString::Printf( TEXT( "ABaseEnemyCtlr moveTo function is being called and is not implemented" ) ) );
#endif
	return EPathFollowingRequestResult::AlreadyAtGoal;
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


void ABaseEnemyCtrl::increaseAttackCounter()
{
	currentAttacksLeftToidle++;
	if (currentAttacksLeftToidle >= attacksToidle)
	{
		gm->combatMan->MaxNumberOfAttacksCompleted(this);
	}


	isExecutingAttack = false;
}

void ABaseEnemyCtrl::removeCtrlFromAttackPool()
{
	gm->combatMan->AttackFinished( this );
}

void ABaseEnemyCtrl::changeCombatState( combatState newState )
{
	combatState previousState = currentCombatState;
	currentCombatState = newState;

	if( newState == combatState::idle && previousState != newState )
	{
		gm->combatMan->addEnemyToIdleList( enemyPawn );
		gm->combatMan->refreshInCombatList();
	}

	if( newState == combatState::inCombat )
	{
		currentAttacksLeftToidle = 0;
	}

	combatStateChanged();
}

void ABaseEnemyCtrl::whenHpGoesTo0( DamageModes type )
{
	gm->combatMan->RemoveFromAllLists(this);
	gm->combatMan->refreshInCombatList();
}

