// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetMarkComponent.h"
#include <Scyther/Enemies/Worm/WormPawn.h>
#include "Components/MaterialBillboardComponent.h"

// Sets default values for this component's properties
UTargetMarkComponent::UTargetMarkComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	targetMark = CreateDefaultSubobject<UMaterialBillboardComponent>( TEXT( "Target Mark Billboard" ) );

	( (UPrimitiveComponent*) targetMark )->bRenderCustomDepth = true;
	( (UPrimitiveComponent*) targetMark )->CustomDepthStencilValue = 2;

	// ...
}


// Called when the game starts
void UTargetMarkComponent::BeginPlay()
{
	Super::BeginPlay();

	targetMark->SetRelativeLocation( GetOwner()->GetActorLocation() + markOffset );
	targetMark->SetVisibility( false, true );

	wormActor = Cast<AWormPawn>( GetOwner() );
	
}


// Called every frame
void UTargetMarkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if( wormActor )
	{
		targetMark->SetWorldLocation( wormActor->wormCapsule->GetComponentLocation() + markOffset);
	}
	else
	{
		targetMark->SetWorldLocation( GetOwner()->GetActorLocation() + markOffset );
	}
	

	//Checks
	if( posibleTargetMarkActive && targetMark->Elements[0].BaseSizeX != posibleMarkSize )
	{
		targetMark->Elements[0].BaseSizeX = posibleMarkSize;
		targetMark->Elements[0].BaseSizeY = posibleMarkSize;
	}

	if( actualTargetAnimationEnabled && actualTargetMarkActive )
	{

		targetMark->SetVisibility( false, true );

		if( animationIsGrowing )
		{
			targetMark->Elements[0].BaseSizeX += 0.4;
			targetMark->Elements[0].BaseSizeY += 0.4;
		}
		else
		{
			targetMark->Elements[0].BaseSizeX -= 0.4;
			targetMark->Elements[0].BaseSizeY -= 0.4;
		}
		
		if( targetMark->Elements[0].BaseSizeX >= actualMarkSize + 6 || targetMark->Elements[0].BaseSizeX <= actualMarkSize - 6 )
		{
			animationIsGrowing = !animationIsGrowing;
		}


		targetMark->SetVisibility( true, true );
	}
}


void UTargetMarkComponent::TogglePosibleTargetMark( bool activate )
{
	if( activate )
	{
		if( !posibleTargetMarkActive )
		{
			posibleTargetMarkActive = true;
			actualTargetMarkActive = false;

			targetMark->SetVisibility( false, true );

			//UE_LOG( LogTemp, Warning, TEXT( "%i" ), targetMark->Elements.Num() );
			if( targetMark->Elements.Num() == 0 )
			{
				FMaterialSpriteElement elem = FMaterialSpriteElement();
				targetMark->Elements.Add( elem );
				targetMark->Elements[0].BaseSizeX = posibleMarkSize;
				targetMark->Elements[0].BaseSizeY = posibleMarkSize;
				//UE_LOG( LogTemp, Warning, TEXT( "%i" ), targetMark->Elements.Num() );
			}
			targetMark->Elements[0].Material = posibleTargetMaterial;
			
			targetMark->SetVisibility( true, true );
		}
	}
	else
	{
		posibleTargetMarkActive = false;
		//actualTargetMarkActive = false;
		if( !actualTargetMarkActive )
		{
			targetMark->SetVisibility( false, true );
		}
	}
}


void UTargetMarkComponent::ToggleActualTargetMark( bool activate )
{
	if( activate )
	{
		if( !actualTargetMarkActive )
		{
			posibleTargetMarkActive = false;
			actualTargetMarkActive = true;

			targetMark->SetVisibility( false, true );

			//UE_LOG( LogTemp, Warning, TEXT( "%i" ), targetMark->Elements.Num() );
			if( targetMark->Elements.Num() != 0 )
			{
				targetMark->Elements.Empty();
			}
			FMaterialSpriteElement elem = FMaterialSpriteElement();
			targetMark->Elements.Add( elem );
			targetMark->Elements[0].BaseSizeX = actualMarkSize;
			targetMark->Elements[0].BaseSizeY = actualMarkSize;
			//UE_LOG( LogTemp, Warning, TEXT( "Cambiando el material" ) );
			targetMark->Elements[0].Material = actualTargetMaterial;
			
			targetMark->SetVisibility( true, true );
		}
	}
	else
	{
		//posibleTargetMarkActive = false;
		actualTargetMarkActive = false;
		if( !posibleTargetMarkActive )
		{
			targetMark->SetVisibility( false, true );
		}
	}
}

