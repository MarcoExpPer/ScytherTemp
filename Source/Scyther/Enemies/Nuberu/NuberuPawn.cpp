// Fill out your copyright notice in the Description page of Project Settings.


#include "NuberuPawn.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"

ANuberuPawn::ANuberuPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	nuberuBody = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "Cuerpo del nuberu" ) );
	SetRootComponent( nuberuBody );

}
//C:/PROYECTO_TFM/Grupo5/Engine/Content/BasicShapes/Cylinder.uasset
void ANuberuPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANuberuPawn::RayAttack( FVector TargetLocation )
{

	AActor* player = Cast<AActor>( UGameplayStatics::GetPlayerPawn( GetWorld(), 0 ));
	TArray<AActor*> ActorsToIgnore;
	//ActorsToIgnore.Add( player );
	
	FHitResult OutHits;
	auto end = TargetLocation;
	
	bool bresult = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		this->GetActorLocation(),
		end,
		TraceTypeQuery_MAX,
		true,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		OutHits,
		true
	);
	if(bresult == true && OutHits.GetActor() == player )
	{
		//implementar daño(Aunque al parecer no va a tener daño sino un efecto de paralizar al player o a un enemigo si le ha dado) y poner efecto niagara hasta el player
		UE_LOG( LogTemp, Warning, TEXT( "Golpea el rayo al player" ) );
	}else
	{
		// o no le da a nada o choca con algo, si no da nada poner el rayo hasta el punto final, si le da a algo poner como fin lo que ha dado
		UE_LOG( LogTemp, Warning, TEXT( "No golpea al player" ) );
	}
}
