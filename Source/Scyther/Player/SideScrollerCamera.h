#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SideScrollerCamera.generated.h"

UCLASS()
class SCYTHER_API ASideScrollerCamera : public AActor
{
	GENERATED_BODY()
	
public:
	// Establece los valores predeterminados para las propiedades de este actor
	ASideScrollerCamera();

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Spring Arm", meta = ( DisplayName = "Spring Arm" ) )
		class USpringArmComponent* springArm;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = ( DisplayName = "Side View Camera" ) )
		class UCameraComponent* playerCamera;

protected:
	// Se llama cuando se inicia el juego o cuando se genera
	virtual void BeginPlay() override;

public:
	// Llamada a cada fotograma
	virtual void Tick(float DeltaTime) override;
};
