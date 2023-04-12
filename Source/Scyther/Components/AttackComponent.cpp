/*****************************************************************************************************/
/***************** Archivo fuente del componente que maneja el ataque del caracter *******************/
/****************************************** Autor: Alvaro ********************************************/
/****************************************** Autor: Miguel ********************************************/
/**************************************** Fecha: 24-03-2023 ******************************************/
/*****************************************************************************************************/

#include "AttackComponent.h"
#include "HealthComponent.h"
#include "TargetingComponent.h"
#include "../Enemies/BaseEnemy.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include <DrawDebugHelpers.h>

#include "Scyther/Enemies/Worm/WormPawn.h"

// Sets default values for this component's properties
UAttackComponent::UAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	character = Cast<AScytherPlayerPawn>( GetOwner() );
	// TODO: Agregar cast de la guadaña y comprobar si la tiene el player
	if( !character )
	{
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra character" ) );
		return;
	}
	targetComponent = character->targetComponent;
	if( !targetComponent )
	{
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra componente target" ) );
		return;
	}
	meshCharacter = character->mesh;
	if( !meshCharacter )
	{
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra mesh del character" ) );
		return;
	}
	scytheAttackCol = character->scytheAttackCol;
	if( !scytheAttackCol )
	{
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentran las colisiones de la guadana" ) );
		return;
	}
	meshAnimation = meshCharacter->GetAnimInstance();
}


// Called every frame
void UAttackComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );


	// Si el jugador está en combo, actualizamos el cooldown, 
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

	// Si esta en medio de alguna animacion de ataque
	// si ha terminado alguna animacion
	if( isOnAttackAnim && animCooldown > 0 )
	{
		animCooldown -= DeltaTime;
	}
	else if( isOnAttackAnim && animCooldown <= 0 )
	{
		isOnAttackAnim = false;
		OnFinishAttack();
	}

	// Si tiene ataques por realizar y no se está realizando ninguna animación justo ahora
	// Si ya se han realizado todos los ataques y no estas realizando ninguna animacion
	if( actualAttackCounter < attackCounter && !isOnAttackAnim )
	{
		//Ejecutamos el siguiente ataque
		actualAttackCounter += 1;
		PlayAnimAttack();
		DoDamage();
	}
	else if( actualAttackCounter >= attackCounter && !isOnAttackAnim )
	{
		actualAttackCounter = 0;
		attackCounter = 0;
	}
}

void UAttackComponent::OnAttack()
{
	//NULL CHECKS
	if( !character || !meshCharacter )
	{
		character->state = MovementState::WALKING;
		UE_LOG( LogTemp, Error, TEXT( "Attack Component: No se encuentra character o mesh" ) );
		return;
	}
	if( !firstAttackAnimMontage || !secondAttackAnimMontage || !finalComboAttackAnimMontage || !airAttackAnimMontage )
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

	//Si se puede atacar en el aire o esta tocando suelo, podemos atacar atacamos
	if( canAttackOnAir || character->isGrounded )
	{
		//Si el player esta en el suelo, hacemos el combo normal
		if( character->isGrounded )
		{

			if( !canMoveWhileAttacking )
			{
				character->state = MovementState::ATTACKING;
			}

			//Si no estaba atacando, pasamos a atacando y activamos el combo
			if( !isAttacking )
			{
				isAttacking = true;
				isComboActivate = true;
			}

			//Si ha atacado en el tiempo del cooldown y no se pasa del maximo de ataques, se añade un ataque más
			if( actualComboCooldown >= 0 && attackCounter < 3 )
			{
				attackCounter += 1;
				actualComboCooldown = comboCooldown;
			}
		}
		//Si esta en el aire, hacemos el ataque aereo
		else
		{
			DoAirAttack();
		}
	}
}

void UAttackComponent::PlayAnimAttack()
{
	// TESTING: Se multiplica por 2.5 por ahora que es la duracion standar de la animacion que tenemos
	switch( actualAttackCounter )
	{
	case 0:
		UE_LOG( LogTemp, Warning, TEXT( "No deberías estar aqui." ) );
	case 1:
		state = AttackState::FIRST;
		meshAnimation->Montage_Play( firstAttackAnimMontage, durationFirstAttackAnimation * modifFirstAttackAnimation );
		isOnAttackAnim = true;
		animCooldown = durationFirstAttackAnimation;
		break;
	case 2:
		state = AttackState::SECOND;
		meshAnimation->Montage_Play( secondAttackAnimMontage, durationSecondAttackAnimation * modifSecondAttackAnimation );
		isOnAttackAnim = true;
		animCooldown = durationSecondAttackAnimation;
		break;
	case 3:
		state = AttackState::FINAL;
		meshAnimation->Montage_Play( finalComboAttackAnimMontage, durationFinalAttackAnimation * modifFinalAttackAnimation );
		isOnAttackAnim = true;
		animCooldown = durationFinalAttackAnimation;
		break;
	default:
		break;
	}
}

void UAttackComponent::DoDamage()
{
	FVector box = FVector( 0, 0, 0 );

	switch( state )
	{
	case AttackState::FIRST:
		switch( firstAttackDirection )
		{
		case AttackDirection::HORIZONTAL:
			box = FVector( firstAttackReach / 2, firstAttackWidth / 2, firstAttackThickness / 2 );
			break;
		case AttackDirection::VERTICAL:
			box = FVector( firstAttackReach / 2, firstAttackThickness / 2, firstAttackWidth / 2 );
			break;
		default:
			break;
		}
		boxColOffset = FVector( firstAttackReach / 2 + 10, 0, 0 ) + firstAttackOffset;
		character->scytheAttackCol->SetBoxExtent( box );
		break;
	case AttackState::SECOND:
		switch( secondAttackDirection )
		{
		case AttackDirection::HORIZONTAL:
			box = FVector( secondAttackReach / 2, secondAttackWidth / 2, secondAttackThickness / 2 );
			break;
		case AttackDirection::VERTICAL:
			box = FVector( secondAttackReach / 2, secondAttackThickness / 2, secondAttackWidth / 2 );
			break;
		default:
			break;
		}
		boxColOffset = FVector( secondAttackReach / 2 + 10, 0, 0 ) + secondAttackOffset;
		character->scytheAttackCol->SetBoxExtent( box );
		break;
	case AttackState::FINAL:
		switch( finalAttackDirection )
		{
		case AttackDirection::HORIZONTAL:
			box = FVector( finalAttackReach / 2, finalAttackWidth / 2, finalAttackThickness / 2 );
			break;
		case AttackDirection::VERTICAL:
			box = FVector( finalAttackReach / 2, finalAttackThickness / 2, finalAttackWidth / 2 );
			break;
		default:
			break;
		}
		boxColOffset = FVector( finalAttackReach / 2 + 10, 0, 0 ) + finalAttackOffset;
		character->scytheAttackCol->SetBoxExtent( box );
		break;
	case AttackState::AIR:
		switch( airAttackDirection )
		{
		case AttackDirection::HORIZONTAL:
			box = FVector( airAttackReach / 2, airAttackWidth / 2, airAttackThickness / 2 );
			break;
		case AttackDirection::VERTICAL:
			box = FVector( airAttackReach / 2, airAttackThickness / 2, airAttackWidth / 2 );
			break;
		default:
			break;
		}
		boxColOffset = FVector( airAttackReach / 2 + 10, 0, 0 ) + airAttackOffset;
		character->scytheAttackCol->SetBoxExtent( box );
		break;
	default:
		break;
	}

	( (USceneComponent*) character->scytheAttackCol )->SetRelativeLocation( boxColOffset );

	character->scytheAttackCol->GetOverlappingActors( damagedEnemies );
	damagedEnemies = GetEnemies( damagedEnemies );

	for( int i = 0; i < damagedEnemies.Num(); i++ )
	{
		UE_LOG( LogTemp, Warning, TEXT( "%s" ), *damagedEnemies[i]->GetName() );
		UHealthComponent* healthComp = (UHealthComponent*) damagedEnemies[i]->GetComponentByClass( UHealthComponent::StaticClass() );
		if( !healthComp )
		{
			UE_LOG( LogTemp, Error, TEXT( "No se ha podido obtener el componente de vida del enemigo %s." ), *damagedEnemies[i]->GetName() );
			return;
		}
		if( isGodMode )
		{
			switch( state )
			{
			case AttackState::FIRST:
				healthComp->receiveDamage( 100000.0f );
				break;

			case AttackState::SECOND:
				healthComp->receiveDamage( 100000.0f );
				break;

			case AttackState::FINAL:
				healthComp->receiveDamage( 100000.0f );
				break;

			case AttackState::AIR:
				healthComp->receiveDamage( 100000.0f );
				break;

			default:
				break;
			}
		}
		else
		{
			switch( state )
			{
			case AttackState::FIRST:
				healthComp->receiveDamage( firstAttackDamage );
				break;

			case AttackState::SECOND:
				healthComp->receiveDamage( secondAttackDamage );
				break;

			case AttackState::FINAL:
				healthComp->receiveDamage( finalAttackDamage );
				break;

			case AttackState::AIR:
				healthComp->receiveDamage( airAttackDamage );
				break;

			default:
				break;
			}
		}
	}

	damagedEnemies.Empty();
}

void UAttackComponent::OnAttackCombo()
{
	if( isComboActivate )
	{
		isComboActivate = false;
		OnFinishAttack();
	}
	else
	{
		OnFinishAttack();
	}
}

void UAttackComponent::DoAirAttack()
{
	PlayAirAnimAttack();
	DoDamage();
}

void UAttackComponent::PlayAirAnimAttack()
{
	state = AttackState::AIR;
	meshAnimation->Montage_Play( airAttackAnimMontage, durationAirAttackAnimation * modifAirAttackAnimation );
	isOnAttackAnim = true;
	animCooldown = durationAirAttackAnimation;
}

void UAttackComponent::OnFinishAttack()
{
	character->state = MovementState::WALKING;
	isAttacking = false;
}

FString UAttackComponent::GetStateAsName( AttackState EnumValue )
{
	const UEnum* EnumPtr = FindObject<UEnum>( ANY_PACKAGE, TEXT( "AttackState" ), true );
	if( !EnumPtr ) return FString( "Invalid" );
	return EnumPtr->GetNameByValue( (int64) EnumValue ).ToString();
}

/// <summary>
/// Gets a list of enemies
/// </summary>
/// <param name="list"> List to search for enemies </param>
TArray<AActor*> UAttackComponent::GetEnemies( TArray<AActor*>& list )
{
	TArray<AActor*> output;
	for( int i = 0; i < list.Num(); i++ )
	{
		if( list[i]->GetClass()->IsChildOf( ABaseEnemy::StaticClass() ) )
		{
			if( list[i]->GetClass()->IsChildOf( AWormPawn::StaticClass() ) )
			{
				TArray<UPrimitiveComponent*> DamageZone;
				character->scytheAttackCol->GetOverlappingComponents( DamageZone );
				for( UPrimitiveComponent* comp : DamageZone )
				{
					if( comp->ComponentHasTag( "DamageZone" ) )
					{
						output.Add( list[i] );
					}
				}
				DamageZone.Empty();
			}
			else
			{
				output.Add( list[i] );
			}

		}
	}
	return output;
}