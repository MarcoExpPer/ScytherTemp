#include "SideScrollerCamera.h"
#include "UObject/Object.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/TargetingComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <DrawDebugHelpers.h>
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Establece valores predeterminados
ASideScrollerCamera::ASideScrollerCamera()
{
	// Establezca esta actor para llamar. Marca() cada fotograma. Puede desactivar esta opción para mejorar el rendimiento si no lo necesita.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera spring attached to the root (capsule)
	springArm = CreateDefaultSubobject<USpringArmComponent>( TEXT( "Spring Arm" ) );
	springArm->bDoCollisionTest = false;
	springArm->TargetArmLength = 500.f;
	springArm->SocketOffset = FVector( 0.f, 0.f, 75.f );
	springArm->SetRelativeRotation( FRotator( 0.f, 180.f, 0.f ) );


	playerCamera = CreateDefaultSubobject<UCameraComponent>( TEXT( "Camera" ) );
	playerCamera->SetupAttachment( springArm );
	playerCamera->bUsePawnControlRotation = false; // We don't want the controller rotating the camera
}

// Se llama cuando se inicia el juego o cuando se genera
void ASideScrollerCamera::BeginPlay()
{
	Super::BeginPlay();
}

// Llamada a cada fotograma
void ASideScrollerCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
