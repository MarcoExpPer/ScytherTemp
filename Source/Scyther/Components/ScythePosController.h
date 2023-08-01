/*****************************************************************************************************/
/********** Archivo fuente del componente que maneja el cambio de posicion de la guadaña *************/
/****************************************** Autor: Miguel ********************************************/
/**************************************** Fecha: 26-06-2023 ******************************************/
/*************************************** Updated: 26-06-2023 *****************************************/
/*****************************************************************************************************/

#pragma once

#include "../Player/ScytherPlayerPawn.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ScythePosController.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCYTHER_API UScythePosController : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FName backSocket;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		FName handSocket;

	UPROPERTY( EditAnywhere, Category = "Tiempo", meta = ( DisplayName = "Tiempo", 
		ToolTip = "Tiempo a esperar antes de guardar la guadaña" ) )
		float timeTokeepScythe = 5.f;

private:

	/// The character
	AScytherPlayerPawn* character = nullptr;

	/// TargetingComponent reference
	UTargetingComponent* targetingComp = nullptr;

	float actualTimeTokeep;

	bool scytheIsOnHand = false;

public:	
	// Sets default values for this component's properties
	UScythePosController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	UFUNCTION( BlueprintCallable )
		void MoveScytheToHand();

	UFUNCTION( BlueprintCallable )
		void MoveScytheToBack();
		
};
