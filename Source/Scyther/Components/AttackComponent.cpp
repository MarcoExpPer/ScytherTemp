/*****************************************************************************************************/
/***************** Archivo fuente del componente que maneja el ataque del caracter *******************/
/****************************************** Autor: Alvaro ********************************************/
/****************************************** Autor: Miguel ********************************************/
/**************************************** Fecha: 24-03-2023 ******************************************/
/*************************************** Updated: 28-05-2023 *****************************************/
/*****************************************************************************************************/

#include "AttackComponent.h"
#include "HealthComponent.h"
#include "DashComponent.h"
#include "TargetingComponent.h"
#include "SkillComponent.h"
#include "3D_MovementComponent.h"
#include "../Player/Habilidades/LightSkillAttack.h"
#include "../Enemies/BaseEnemy.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Math/UnrealMathUtility.h"
#include "Components/BoxComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "CollisionQueryParams.h"
#include <DrawDebugHelpers.h>
#include "Scyther/Enemies/Worm/WormPawn.h"

// Sets default values for this component's properties
UAttackComponent::UAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	state = AttackState::NONE;
}


////////////////////////////////////////////////////
////////		 BEGIN PLAY & TICK			////////
////////////////////////////////////////////////////



// Called when the game starts
void UAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	//Player
	character = Cast<AScytherPlayerPawn>( GetOwner() );
	if( !character )
	{
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra character" ) );
		return;
	}

	//Skill Component
	skillComponent = character->skillComponent;
	if( !skillComponent )
	{
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra componente skill" ) );
		return;
	}

	//Mesh
	meshCharacter = character->mesh;
	if( !meshCharacter )
	{
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra mesh del character" ) );
		return;
	}

	//Scythe Box Collision
	scytheAttackCol = character->scytheAttackCol;
	if( !scytheAttackCol )
	{
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentran las colisiones de la guadana" ) );
		return;
	}
	scytheAttackCol->OnComponentBeginOverlap.AddDynamic( this, &UAttackComponent::ScytheBeginCollision );

	//VerticalAttack Capsule Colision
	verticalAttackCol = character->verticalAttackCol;
	if( !verticalAttackCol )
	{
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentran las colisiones de la guadana" ) );
		return;
	}
	verticalAttackCol->OnComponentBeginOverlap.AddDynamic( this, &UAttackComponent::VerticalAttackBeginCollision );

	meshAnimation = meshCharacter->GetAnimInstance();
	if( !meshAnimation )
	{
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentran la malla de animacion del character" ) );
		return;
	}

	//VFX Hit
	if( !flashHitDestructibleVFX || !flashHitNotDestructibleVFX )
	{
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentran el VFX del golpe de la guadana destructible o no destructible" ) );
		return;
	}
}


// Called every frame
void UAttackComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	//Actualizamos el cooldown de si hay que esperar para empezar otro combo
	if( attackFinishCooldown > 0 )
	{
		attackFinishCooldown -= DeltaTime;
	}

	//Actualizamos el cooldown de penalizaci?n si hay
	if( actualPenalty > 0 )
	{
		actualPenalty -= DeltaTime;
	}

	if( isDroping && character->isGrounded )
	{
		StopDrop();
	}

	// Si el jugador est? en combo, actualizamos el cooldown, 
	// si el tiempo de cooldown se ha agotado, cortamos el combo
	if( isComboActivate && actualComboCooldown >= 0 )
	{
		actualComboCooldown -= DeltaTime;
	}
	else if( isComboActivate && actualComboCooldown < 0 )
	{
		isComboActivate = false;
		actualComboCooldown = comboCooldown;
	}

	// Si el jugador esta desplazandose
	// lo movemos la velocidad determinada por el estado
	if(  character->movementComp->isMovementActive )
	{
		switch( shiftState )
		{
		case ShiftType::SHIFT_AND_MOVE:
		{
			ShiftAndMoveToTarget( DeltaTime );
			break;
		}
		case ShiftType::SHIFT_AND_ROTATE:
		{
			AWormPawn* wormPawn = Cast<AWormPawn>( enemyToShiftTo );
			FVector targetLocation;

			if( wormPawn )
			{
				targetLocation = FVector( wormPawn->wormCapsule->GetComponentLocation().X, wormPawn->wormCapsule->GetComponentLocation().Y, character->GetActorLocation().Z );
			}
			else
			{
				targetLocation = enemyToShiftTo->GetActorLocation();
			}

			ShiftAndRotateToTarget( DeltaTime, targetLocation );
			break;
		}
		default:
		case ShiftType::NO_SHIFT:
		{
			break;
		}
			
		}
	}

	if( character->state == MovementState::ATTACKING )
	{
		if( attackingStateCooldown >= 0 )
		{
			attackingStateCooldown -= DeltaTime;
		}
		else
		{
			character->state = MovementState::WALKING;
			attackingStateCooldown = 0.65f;
		}
	}
	else
	{
		shiftState = ShiftType::NO_SHIFT;
	}

	if( isAttacking )
	{
		TArray<AActor*> enemies = GetDamagedEnemies();
		//Hacemos da?o a los enemigos y que retrocedan al golpear
		for(AActor* enemy : enemies )
		{
			if( !damagedEnemies.Contains( enemy->GetName() ))
			{
				HitEnemy( enemy );
				damagedEnemies.Add( enemy->GetName() );
			}
		}
	}


#if WITH_EDITOR 
	if( attack1AreaDebug && isDoingDamage && state == AttackState::FIRST )
	{
		DrawDebugBox( GetWorld(), scytheAttackCol->GetComponentLocation(), scytheAttackCol->GetUnscaledBoxExtent(), FColor::Purple, false, 1.f );
	}
	if( attack2AreaDebug && isDoingDamage && state == AttackState::SECOND)
	{
		DrawDebugBox( GetWorld(), scytheAttackCol->GetComponentLocation(), scytheAttackCol->GetUnscaledBoxExtent(), FColor::Purple, false, 1.f );
	}
	if( attack3AreaDebug && isDoingDamage && state == AttackState::THIRD )
	{
		DrawDebugBox( GetWorld(), scytheAttackCol->GetComponentLocation(), scytheAttackCol->GetUnscaledBoxExtent(), FColor::Purple, false, 1.f );
	}
	if( attackAirAreaDebug && isDoingDamage && state == AttackState::AIR )
	{
		DrawDebugBox( GetWorld(), scytheAttackCol->GetComponentLocation(), scytheAttackCol->GetUnscaledBoxExtent(), FColor::Purple, false, 1.f );
	}
#endif

}

void UAttackComponent::ShiftAndMoveToTarget( float DeltaTime )
{
	if( !playerShiftFollowsEnemy )
	{
		FVector dir = GetOwner()->GetActorForwardVector();
		ShiftWithoutTarget( DeltaTime, dir );
		return;
	}

	AWormPawn* wormPawn = Cast<AWormPawn>( enemyToShiftTo );
	FVector targetLocation;

	if( wormPawn )
	{
		targetLocation = FVector( wormPawn->wormCapsule->GetComponentLocation().X, wormPawn->wormCapsule->GetComponentLocation().Y, character->GetActorLocation().Z );
	}
	else
	{
		targetLocation = enemyToShiftTo->GetActorLocation();
	}


	ShiftAndRotateToTarget( DeltaTime, targetLocation );

	//Esta comprobacion en principio no hace falta, pero por si acaso
	if( enemyToShiftTo != nullptr && enemyToShiftTo->healthComp->currentHp > 0 )
	{
		float distanceToLocation = (character->GetActorLocation() - targetLocation).Size();
		float minDistance = state == AttackState::FIRST ? firstAttackShiftMinDistance :
					state == AttackState::SECOND ? secondAttackShiftMinDistance : finalAttackShiftMinDistance;

		if( distanceToLocation < minDistance )
		{
			character->movementComp->velocity = FVector::ZeroVector;
			shiftState = ShiftType::NO_SHIFT;
		}
		else
		{
			float shifhtSpeed = state == AttackState::FIRST ? firstAttackShiftSpeed :
						state == AttackState::SECOND ? secondAttackShiftSpeed : finalAttackShiftSpeed;

			FVector shiftDirection = ( targetLocation - character->GetActorLocation() ).GetSafeNormal();

			character->movementComp->AddHorizontalMovement( shiftDirection, 1 * shifhtSpeed * DeltaTime, false );
		}
	}
	else
	{
		character->movementComp->velocity = FVector::ZeroVector;
		shiftState = ShiftType::NO_SHIFT;
	}
	}

void UAttackComponent::ShiftAndRotateToTarget(float DeltaTime, FVector targetLocation )
{
	//Esta comprobacion en principio no hace falta, pero por si acaso
	if( enemyToShiftTo != nullptr && enemyToShiftTo->healthComp->currentHp > 0 )
	{
		FVector vectorToenemy = targetLocation - character->GetActorLocation();
		character->movementComp->rotateTowardsVector( DeltaTime, vectorToenemy );
	}
}

void UAttackComponent::ShiftWithoutTarget( float DeltaTime, FVector& dir )
{
	float distToShift = firstAttackShiftDistance;
	if( state == AttackState::SECOND )
	{
		distToShift = secondAttackShiftDistance;
	}
	else
	{
		distToShift = finalAttackShiftDistance;
	}

	switch( state )
	{
	case AttackState::FIRST:
		character->movementComp->AddHorizontalMovement( DeltaTime * dir, firstAttackShiftSpeed, false );
		shiftedDist += ( firstAttackShiftSpeed * DeltaTime * dir ).Size();
		break;

	case AttackState::SECOND:
		character->movementComp->AddHorizontalMovement( DeltaTime * dir, secondAttackShiftSpeed, false );
		shiftedDist += ( secondAttackShiftSpeed * DeltaTime * dir ).Size();
		break;

	case AttackState::THIRD:
		character->movementComp->AddHorizontalMovement( DeltaTime * dir, finalAttackShiftSpeed, false );
		shiftedDist += ( finalAttackShiftSpeed * DeltaTime * dir ).Size();
		break;

	default:
		break;
	}

	if( shiftedDist >= distToShift )
	{
		shiftState = ShiftType::NO_SHIFT;
		shiftedDist = 0.f;
	}
}




////////////////////////////////////////////////////
////////		   PLAYER INPUT				////////
////////////////////////////////////////////////////

/// <summary>
/// Funcition binded to the player input in the player character
/// </summary>
void UAttackComponent::OnAttack()
{
	//NULL CHECKS
	if( !character || !meshCharacter )
	{
		character->state = MovementState::WALKING;
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra character o mesh" ) );
		return;
	}
	if( !ComboAttackAnimMontage || !airAttackAnimMontage )
	{
		character->state = MovementState::WALKING;
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra alguna animacion" ) );
		return;
	}
	if( !meshAnimation )
	{
		character->state = MovementState::WALKING;
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra la animacion de la mesh del caracter" ) );
		return;
	}

	if( character->state == MovementState::NONE )
	{
		return;
	}

	//Si el player esta en el suelo, hacemos el combo normal
	if( character->isGrounded )
	{
		DoAttackCombo();
	}
	//Si esta en el aire y tenemos el ataque aereo activado
	else if( canAttackOnAir )
	{
		DoAirAttack();
	}
}



////////////////////////////////////////////////////
////////		   ANIM NOTIFIES			////////
////////////////////////////////////////////////////


/// <summary>
/// Anim Notify when the attack has ended
/// Based on the state, it makes the next attack or finishes the combo
/// </summary>
void UAttackComponent::OnAttackEnded()
{
	shiftState = ShiftType::NO_SHIFT;
	//Combo
	if( state != AttackState::AIR )
	{
		if( character->state != MovementState::KNOCKBACK && character->dashComponent->CheckDashInputSaved() )
		{
			FinishAttackCombo();
			return;
		}

		if( (int) state >= attackCounter )
		{
			FinishAttackCombo();
		}
		else
		{
			DoNextAttack();
		}

		
	}
	//Ataque aereo
	else
	{
		FinishAirAttack();
	}

	if( character->state != MovementState::KNOCKBACK )
	{
		character->ChangeMovementState( MovementState::WALKING );
	}
}

/// <summary>
/// Anim Notify when the player starts to shift before the attack
/// Based on the state, it updates the player state to make it move
/// </summary>
void UAttackComponent::MakeShift()
{
	if( !playerShiftFollowsEnemy )
	{
		shiftState = ShiftType::SHIFT_AND_MOVE;
		return;
	}

	//Look for the target location that may be shifted to
	if( character->targetComponent->playerTarget )
	{
		enemyToShiftTo = Cast<ABaseEnemy>(character->targetComponent->playerTarget);
		if( isEnemyInShiftRange() )
		{
			return;
		}
	}
	else
	{
		//En el primer ataque, siempre buscamos al enemigo mas cercano
		if( state == AttackState::FIRST )
		{
			if( !UpdateEnemyToShitToWithClosestEnemy() )
			{
				return;
			}
		}
		//En el segundo y tercer ataque, comprobamos si el enemigo al que hemos atacado antes sigue en rango
		else
		{
			if( enemyToShiftTo != nullptr && enemyToShiftTo->healthComp->currentHp > 0 )
			{	
				if( isEnemyInShiftRange() )
				{
					return;
				}
			}
			//Si no sigue en rango, volvemos a buscar el enemigo mas cercano
			if( !UpdateEnemyToShitToWithClosestEnemy() )
			{
				return;
			}
		}
		//Comprobamos si el enemigo mas cercano esta en rango
		if( isEnemyInShiftRange() )
		{
			return;
		}
	}
}


bool UAttackComponent::UpdateEnemyToShitToWithClosestEnemy()
{
	enemyToShiftTo = Cast<ABaseEnemy>( character->targetComponent->actualClosestEnemy );
	if( enemyToShiftTo == nullptr )
	{
		shiftState = ShiftType::NO_SHIFT;
		return false;
	}
	return true;
}

bool UAttackComponent::isEnemyInShiftRange()
{
	if( enemyToShiftTo == nullptr )
	{
		shiftState = ShiftType::NO_SHIFT;
		return false;
	}

	AWormPawn* wormPawn = Cast<AWormPawn>( enemyToShiftTo );
	FVector targetLocation;

	if( wormPawn )
	{
		targetLocation = FVector( wormPawn->wormCapsule->GetComponentLocation().X, wormPawn->wormCapsule->GetComponentLocation().Y, character->GetActorLocation().Z );
	}
	else
	{
		targetLocation = enemyToShiftTo->GetActorLocation();
	}


	FVector enemyXY = FVector( targetLocation.X, targetLocation.Y, character->GetActorLocation().Z );

	float distanceToEnemy = ( enemyXY - character->GetActorLocation() ).Size();

	float maxDistance = state == AttackState::FIRST ? firstAttackShiftMaxDistance :
		state == AttackState::SECOND ? secondAttackShiftMaxDistance : finalAttackShiftMaxDistance;
	float minDistance = state == AttackState::FIRST ? firstAttackShiftMinDistance :
		state == AttackState::SECOND ? secondAttackShiftMinDistance : finalAttackShiftMinDistance;

	if( distanceToEnemy <= minDistance )
	{
		shiftState = ShiftType::SHIFT_AND_ROTATE;
		return true;
	}
	else if( distanceToEnemy <= maxDistance )
	{
		shiftState = ShiftType::SHIFT_AND_MOVE;
		return true;
	}
	else
	{
		shiftState = ShiftType::NO_SHIFT;
		return false;
	}
}

/// <summary>
/// Anim Notify when the player stops moving
/// It changes the player movement state to make it stops
/// </summary>
void UAttackComponent::OnMovementStops()
{
	//Combo
	if( state != AttackState::AIR )
	{
		if( !canMoveWhileAttacking )
		{
			character->ChangeMovementState( MovementState::ATTACKING );
			attackingStateCooldown = 0.65f;
		}
	}
	//Ataque aereo
	else
	{
		character->state = MovementState::PREPARING_DROP;
	}

	MakeShift();
}

/// <summary>
/// Anim Notify when the player starts falling from the air attack
/// It updates de player state to make it fall
/// </summary>
void UAttackComponent::OnVerticalDrop()
{
	character->state = MovementState::VERTICAL_DROP;
	isDroping = true;
	meshAnimation->Montage_Pause();

	verticalAttackCol->SetGenerateOverlapEvents( true );
}

/// <summary>
/// Anim Notify: The moment to start checking colisions and making damage
/// </summary>
void UAttackComponent::OnAttackDamageStart()
{
	isDoingDamage = true;

	//Aplicamos el tama?o de colisiones adecuado para el ataque 
	SetScytheCollisionBox();

	//Obtenemos los enemigos golpeados
	TArray<AActor*> enemies = GetDamagedEnemies();

	//activamos las colisiones
	scytheAttackCol->SetGenerateOverlapEvents( true );

	//de momento, el ataque ha fallado. Si mas adelante en la animaci?n golpea algo, se actualizar?.
	if( enemies.Num() == 0 )
	{
		attackMissed = true;
	}

	//Hacemos da?o a los enemigos y que retrocedan al golpear
	for( int i = 0; i < enemies.Num(); i++ )
	{
		HitEnemy( enemies[i] );
		damagedEnemies.Add( enemies[i]->GetName() );
	}
}


/// <summary>
/// Anim Notify: The moment to end checking collisions to make damage
/// </summary>
void UAttackComponent::OnAttackDamageEnd()
{
	isDoingDamage = false;

	//Stops collisions
	scytheAttackCol->SetGenerateOverlapEvents( false );

	//Si al finalizar el ataque no ha golpeado nada, aplicamos la penalizaci?n 
	if( attackMissed )
	{
		ApplyPenalty();
	}

	//reset
	attackMissed = false;
	damagedEnemies.Empty();
}

/// <summary>
/// Anim Notify: OnLightSkill
/// If Light Skill is active, it makes the light attack
/// </summary>
void UAttackComponent::OnLightSkill()
{
	if( skillComponent->GetActiveSkill() == SkillType::LIGHT )
	{
		ALightSkillAttack* attackEffect = (ALightSkillAttack*) GetWorld()->SpawnActor( ALightSkillAttack::StaticClass(), &GetOwner()->GetActorTransform() );
		if( attackEffect )
		{
			FVector dir = GetOwner()->GetActorForwardVector();
			float duration = skillComponent->lightProyectileDuration;
			float damage = skillComponent->lightProjectileDamage;
			float width = skillComponent->lightProyectileWidth;
			float height = skillComponent->lightProjectileHeight;
			float speed = skillComponent->lightProyectileSpeed;
			UNiagaraSystem* vfx = skillComponent->lightSkillAttackVFX;
			attackEffect->InitLightSkillAttackValues( dir, duration, damage, width, height, speed, vfx, false );
			attackEffect->StartVFX();
		}
		if( character->skillComponent->actualLightSkillAttacks > 2 )
		{
			ALightSkillAttack* attackEffect2 = (ALightSkillAttack*) GetWorld()->SpawnActor( ALightSkillAttack::StaticClass(), &GetOwner()->GetActorTransform() );
			if( attackEffect2 )
			{
				FVector dir = GetOwner()->GetActorForwardVector();
				float duration = skillComponent->lightProyectileDuration;
				float damage = skillComponent->lightProjectileDamage;
				float width = skillComponent->lightProyectileWidth;
				float height = skillComponent->lightProyectileWidth;
				float speed = skillComponent->lightProyectileSpeed;
				UNiagaraSystem* vfx = skillComponent->lightSkillAttackVFX;
				attackEffect2->InitLightSkillAttackValues( dir, duration, damage, width, height, speed, vfx, true );
				attackEffect2->StartVFX();
			}
		}
		skillComponent->DisabledSkill();
	}
}




////////////////////////////////////////////////////
////////		  COMBO DE ATAQUE			////////
////////////////////////////////////////////////////



/// <summary>
/// Makes the changes needed to store the input buffer of the combo and starts the combo if necesary
/// </summary>
void UAttackComponent::DoAttackCombo()
{
	//Comprobamos que no estemos en cooldown por combo o que no tengamos penalizacion
	if( attackFinishCooldown <= 0 && actualPenalty <= 0 )
	{
		//Si no estaba atacando, pasamos a atacando y activamos el combo
		if( state == AttackState::NONE && attackCounter == 0 )
		{
			isAttacking = true;
			isComboActivate = true;
			attackCounter++;
		}

		if( state == AttackState::FIRST && attackCounter == 1 )
		{
			attackCounter++;
		}

		if( state == AttackState::SECOND && attackCounter == 2 )
		{
			attackCounter++;
		}

		//Primer ataque, comenzamos el combo
		if( attackCounter == 1 )
		{
			character->skillComponent->lightSkillAttacks = 0;
			StartAttackCombo();
		}
	}
}


/// <summary>
/// Starts the combo from the first attack
/// </summary>
void UAttackComponent::StartAttackCombo()
{
	state = AttackState::FIRST;
	PlayComboAnimAttack();
}


/// <summary>
/// Finish the combo
/// </summary>
void UAttackComponent::FinishAttackCombo()
{
	meshAnimation->StopAllMontages( 0.25f );
	isAttacking = false;

	switch( state )
	{
	case AttackState::FIRST:
		attackFinishCooldown = firstAttackFinishCooldown;
		break;
	case AttackState::SECOND:
		attackFinishCooldown = secondAttackFinishCooldown;
		break;
	case AttackState::THIRD:
		attackFinishCooldown = finalAttackFinishCooldown;
		break;
	default:
		break;
	}
	state = AttackState::NONE;
	attackCounter = 0;
}



/// <summary>
/// Based on the state of the attack, plays the corresponding animation
/// </summary>
void UAttackComponent::PlayComboAnimAttack()
{
	switch( state )
	{
	case AttackState::NONE:
		UE_LOG( LogTemp, Warning, TEXT( "No deber?as estar aqui." ) );
		break;
	case AttackState::FIRST:
		meshAnimation->Montage_Play( ComboAttackAnimMontage, modifFirstAttackAnimation );
		meshAnimation->Montage_JumpToSection( "FirstAttack" );
		break;
	case AttackState::SECOND:
		meshAnimation->Montage_Play( ComboAttackAnimMontage, modifSecondAttackAnimation );
		meshAnimation->Montage_JumpToSection( "SecondAttack" );
		break;
	case AttackState::THIRD:
		meshAnimation->Montage_Play( ComboAttackAnimMontage, modifFinalAttackAnimation );
		meshAnimation->Montage_JumpToSection( "ThirdAttack" );
		break;
	default:
		break;
	}
}


/// <summary>
/// Update the state to the next attack
/// </summary>
void UAttackComponent::DoNextAttack()
{
	switch( state )
	{
	case AttackState::FIRST:
		state = AttackState::SECOND;
		break;
	case AttackState::SECOND:
		state = AttackState::THIRD;
		break;
	default:
		break;
	}
	PlayComboAnimAttack();
}

/// <summary>
/// Updates the current penalty to mactch the new situation
/// </summary>
void UAttackComponent::ApplyPenalty()
{
	switch( state )
	{
	case AttackState::FIRST:
		if( firstAttackHasPenalty )
		{
			ApplySpecificPenalty( firstAttackPenalty );
		}
		if( comboBreakWhileLightSkill && firstAttackBreaksCombo )
		{
			CancelAttack( false );
		}
		break;
	case AttackState::SECOND:
		if( secondAttackHasPenalty )
		{
			ApplySpecificPenalty( secondAttackPenalty );
		}
		if( comboBreakWhileLightSkill && secondAttackBreaksCombo )
		{
			CancelAttack( false );
		}
		break;
	case AttackState::THIRD:
		if( finalAttackHasPenalty )
		{
			ApplySpecificPenalty( finalAttackPenalty );
		}
		if( comboBreakWhileLightSkill && finalAttackBreaksCombo )
		{
			CancelAttack( false );
		}
		break;
	default:
		break;
	}
}


void UAttackComponent::MakeSingleAttack()
{
	isAttacking = true;
	isComboActivate = true;
	attackCounter = 1;
	actualComboCooldown = comboCooldown;
	StartAttackCombo();
}


////////////////////////////////////////////////////
////////			ATAQUE AEREO			////////
////////////////////////////////////////////////////

void UAttackComponent::DoAirAttack()
{
	if( !isAttacking )
	{
		isAttacking = true;
		PlayAirAnimAttack();
	}
}

void UAttackComponent::PlayAirAnimAttack()
{
	state = AttackState::AIR;
	meshAnimation->Montage_Play( airAttackAnimMontage, modifAirAttackAnimation );
}

void UAttackComponent::StopDrop()
{
	//Play ForceFeedback
	( (APlayerController*) character->GetController() )->ClientPlayForceFeedback( airGroundHitForceFeedbackEffect );
	//Stop animation
	isDroping = false;
	meshAnimation->Montage_Resume( nullptr );
	verticalAttackCol->SetGenerateOverlapEvents( false );
}


void UAttackComponent::FinishAirAttack()
{
	meshAnimation->StopAllMontages( 0.25f );
	isAttacking = false;
	state = AttackState::NONE;
}



////////////////////////////////////////////////////
////////		 DAMAGE & COLLISION			////////
////////////////////////////////////////////////////

//Begin Collision for the Scythe Box Collision
void UAttackComponent::ScytheBeginCollision( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	//Es un enemigo o un destructible
	if( isADamageableEnemyComp( OtherActor, OtherComp ) )
	{
		//No ha sido da?ado antes
		if( !damagedEnemies.Contains( OtherActor->GetName() ) )
		{
			//Caso del gusano
			if( OtherActor->GetClass()->IsChildOf( AWormPawn::StaticClass() ) )
			{
				//Hemos golpeado en su zona de da?o
				if( OtherComp->ComponentHasTag( "DamageZone" ) )
				{
					GetImpactPointHitToVFX( OverlappedComp, OtherComp );
					attackMissed = false;
					HitEnemy( OtherActor );
					damagedEnemies.Add( OtherActor->GetName() );
				}
				//Hemos golpeado en otra parte
				else
				{
					return;
				}
			}
			//Resto de enemigos/destructibles
			else
			{
				GetImpactPointHitToVFX( OverlappedComp, OtherComp );
				attackMissed = false;
				HitEnemy( OtherActor );
				damagedEnemies.Add( OtherActor->GetName() );
			}
		}
	}
}

//Begin Collision for the Vertical Attack Capsule Collision
void UAttackComponent::VerticalAttackBeginCollision( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	//Es un enemigo o un destructible
	if( isADamageableEnemyComp( OtherActor, OtherComp ) )
	{
		//No ha sido da?ado antes
		if( !damagedEnemies.Contains( OtherActor->GetName() ) )
		{
			//Caso del gusano
			if( OtherActor->GetClass()->IsChildOf( AWormPawn::StaticClass() ) )
			{
				//Hemos golpeado en su zona de da?o
				if( OtherComp->ComponentHasTag( "DamageZone" ) )
				{
					BodyHitEnemy( OtherActor );
					damagedEnemies.Add( OtherActor->GetName() );
				}
				//Hemos golpeado en otra parte
				else
				{
					return;
				}
			}
			//Resto de enemigos/destructibles
			else
			{
				GetImpactPointHitToVFX( OverlappedComp, OtherComp );
				BodyHitEnemy( OtherActor );
				damagedEnemies.Add( OtherActor->GetName() );
			}
			GEngine->AddOnScreenDebugMessage( -1, 2, FColor::Red, FString::Printf( TEXT( "Vertical Attack: Chocado" ) ) );
		}
	}
}


/// <summary>
/// Returns a list of all enemy actors colliding at that frame with the scythe collision.
/// </summary>
/// <returns></returns>
TArray<AActor*> UAttackComponent::GetDamagedEnemies()
{

	TArray<UPrimitiveComponent*> comps;
	scytheAttackCol->GetOverlappingComponents( comps );

	TArray<AActor*> output = GetEnemies( comps );

	return output;
}

void UAttackComponent::HitEnemy( AActor* target )
{
	if( target->GetClass()->IsChildOf( ABaseEnemy::StaticClass() ) )
	{
		if( !( (ABaseEnemy*) target )->isHiteable )
		{
			return;
		}
	}

	UHealthComponent* healthComp = (UHealthComponent*) target->GetComponentByClass( UHealthComponent::StaticClass() );
	if( !healthComp )
	{
		UE_LOG( LogTemp, Error, TEXT( "No se ha podido obtener el componente de vida de %s." ), *target->GetName() );
		return;
	}
	if( isGodMode )
	{
		healthComp->receiveDamage( 100000.0f );
	}
	else
	{
		switch( state )
		{
		case AttackState::FIRST:
			( (APlayerController*) character->GetController() )->ClientPlayForceFeedback( firstAttackForceFeedbackEffect );
			healthComp->receiveDamage( firstAttackDamage );
			hittedEnemyEvent.Broadcast( target->GetName(), character->GetActorLocation(), AttackState::FIRST );
			break;

		case AttackState::SECOND:
			( (APlayerController*) character->GetController() )->ClientPlayForceFeedback( secondAttackForceFeedbackEffect );
			healthComp->receiveDamage( secondAttackDamage );
			hittedEnemyEvent.Broadcast( target->GetName(), character->GetActorLocation(), AttackState::SECOND );
			break;

		case AttackState::THIRD:
			( (APlayerController*) character->GetController() )->ClientPlayForceFeedback( finalAttackForceFeedbackEffect );
			healthComp->receiveDamage( finalAttackDamage );
			hittedEnemyEvent.Broadcast( target->GetName(), character->GetActorLocation(), AttackState::THIRD );
			break;

		case AttackState::AIR:
			( (APlayerController*) character->GetController() )->ClientPlayForceFeedback( airAttackForceFeedbackEffect );
			healthComp->receiveDamage( airAttackDamage );
			hittedEnemyEvent.Broadcast( target->GetName(), character->GetActorLocation(), AttackState::AIR );
			break;
		default:
			break;
		}
	}
}

void UAttackComponent::BodyHitEnemy( AActor* target )
{
	UHealthComponent* healthComp = (UHealthComponent*) target->GetComponentByClass( UHealthComponent::StaticClass() );
	if( !healthComp )
	{
		UE_LOG( LogTemp, Error, TEXT( "No se ha podido obtener el componente de vida de %s." ), *target->GetName() );
		return;
	}
	if( isGodMode )
	{
		healthComp->receiveDamage( 100000.0f );
	}
	else
	{
		healthComp->receiveDamage( airAttackBodyHitDamage );
		hittedEnemyEvent.Broadcast( target->GetName(), character->GetActorLocation(), AttackState::BODY );
	}
}

void UAttackComponent::SetScytheCollisionBox()
{
	FVector box = FVector( 0, 0, 0 );

	switch( state )
	{
	case AttackState::FIRST:
		box = FVector( firstAttackReach / 2, firstAttackWidth / 2, firstAttackThickness / 2 );
		boxColOffset = firstAttackOffset;
		scytheAttackCol->SetBoxExtent( box );
		break;
	case AttackState::SECOND:
		box = FVector(  secondAttackReach / 2, secondAttackWidth / 2, secondAttackThickness / 2 );
		boxColOffset = secondAttackOffset;
		scytheAttackCol->SetBoxExtent( box );
		break;
	case AttackState::THIRD:
		box = FVector( finalAttackReach / 2, finalAttackWidth / 2, finalAttackThickness / 2 );
		boxColOffset = finalAttackOffset;
		scytheAttackCol->SetBoxExtent( box );
		break;
	case AttackState::AIR:
		box = FVector( airAttackReach / 2, airAttackWidth / 2, airAttackThickness / 2 );
		boxColOffset = airAttackOffset;
		scytheAttackCol->SetBoxExtent( box );
		break;
	default:
		break;
	}

	( (USceneComponent*) scytheAttackCol )->SetRelativeLocation( boxColOffset );
}

void UAttackComponent::GetImpactPointHitToVFX( UPrimitiveComponent* OtherActor, UPrimitiveComponent* OtherComp )
{
	if( OtherActor && OtherComp )
	{
		TArray<FHitResult> Results;
		auto Start = OtherActor->GetComponentLocation();
		auto End = OtherComp->GetComponentLocation();
		auto CollisionRadius = FVector::Dist( Start, End ) * 1.2f;

		// spherical sweep 
		GetWorld()->SweepMultiByObjectType( Results, Start, End,
			FQuat::Identity, 0,
			FCollisionShape::MakeSphere( CollisionRadius ),
			// use custom params to reduce the search space
			FCollisionQueryParams::FCollisionQueryParams( FName( "VFXimpact" ), false, nullptr )
		);

		for( auto HitResult : Results )
		{
			bool isDestruible;
			AActor* hitActor = HitResult.GetActor();

			if( hitActor == nullptr )
			{
				break;
			}

			FProperty* prop = hitActor->GetClass()->FindPropertyByName(FName("IsDestruible"));
			if( prop )
			{
				FBoolProperty* boolProp = Cast<FBoolProperty>( prop );
				if( boolProp )
				{
					isDestruible = boolProp->GetPropertyValue_InContainer( HitResult.GetActor() );
				}
			}
			else
			{
				isDestruible = true;
			}
			if( OtherComp->GetUniqueID() == HitResult.GetComponent()->GetUniqueID() )
			{
				UNiagaraSystem* flashHitVFX = isDestruible ? flashHitDestructibleVFX : flashHitNotDestructibleVFX;
				UNiagaraFunctionLibrary::SpawnSystemAtLocation( GetWorld(), flashHitVFX, HitResult.ImpactPoint );
			}
		}
	}
}

bool UAttackComponent::isADamageableEnemyComp( AActor* actor, UPrimitiveComponent* componentHit = nullptr )
{
	if( actor->GetClass()->IsChildOf( DestructibleClass ) )
	{
		return true;
	}

	if( actor->GetClass()->IsChildOf( SecondDestructibleClass ) )
	{
		return true;
	}

	if( componentHit->ComponentHasTag( "EnemyBody" ) )
	{
		return true;
	}

	return false;
}


/// <summary>
/// Gets a list of enemies
/// </summary>
/// <param name="list"> List to search for enemies </param>
TArray<AActor*> UAttackComponent::GetEnemies( TArray<UPrimitiveComponent*>& list )
{
	TArray<AActor*> output;
	for( int i = 0; i < list.Num(); i++ )
	{
		if( isADamageableEnemyComp( list[i]->GetOwner(), list[i] ) )
		{
			if( list[i]->GetOwner()->GetClass()->IsChildOf( AWormPawn::StaticClass() ) )
			{
				TArray<UPrimitiveComponent*> DamageZone;
				scytheAttackCol->GetOverlappingComponents( DamageZone );
				for( UPrimitiveComponent* comp : DamageZone )
				{
					if( comp->ComponentHasTag( "DamageZone" ) )
					{
						output.Add( list[i]->GetOwner() );
					}
				}
				DamageZone.Empty();
			}
			else
			{
				output.Add( list[i]->GetOwner() );
			}
		}
	}
	return output;
}



////////////////////////////////////////////////////
////////		 FUNCIONES EXTRA			////////
////////////////////////////////////////////////////

//Stops the current attack, combo and animation
void UAttackComponent::CancelAttack( bool stopAnimation )
{
	actualComboCooldown = 0.f;
	attackCounter = 0;
	isAttacking = false;
	isComboActivate = false;
	isDroping = false;
	attackMissed = false;
	shiftState = ShiftType::NO_SHIFT;

	if( stopAnimation )
	{
		ApplySpecificPenalty( penaltyWhenHitted );

		meshAnimation->StopAllMontages( 0.15f );
		state = AttackState::NONE;
		character->state = MovementState::WALKING;
		scytheAttackCol->SetGenerateOverlapEvents( false );
		verticalAttackCol->SetGenerateOverlapEvents( false );
		damagedEnemies.Empty();
	}
}

void UAttackComponent::ApplySpecificPenalty( float penalty )
{
	actualPenalty = penalty;
}

FString UAttackComponent::GetStateAsName( AttackState EnumValue )
{
	const UEnum* EnumPtr = FindObject<UEnum>( ANY_PACKAGE, TEXT( "AttackState" ), true );
	if( !EnumPtr ) return FString( "Invalid" );
	return EnumPtr->GetNameByValue( (int64) EnumValue ).ToString();
}