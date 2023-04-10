// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_UpdateNextTargetPoint.h"
#include "AIWormCppController.h"

//Si se llama a esta tarea, devuelve un Succeeded, Failed o InProgress
EBTNodeResult::Type UBTT_UpdateNextTargetPoint::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	
	//Obtenemos la referencia al AIEnemyController
	AAIWormCppController* AIEnemyController = Cast<AAIWormCppController>( OwnerComp.GetOwner() );

	//Llamamos a UpdateNextTargetPoint para seleccionar el siguiente TargetPoint
	AIEnemyController->UpdateNextTargetPoint();

	//Finalmente retornamos Succeeded
	return EBTNodeResult::Succeeded;

}

//Permite añadir una descripción a esta tarea. Este texto se verá en el B.T
FString UBTT_UpdateNextTargetPoint::GetStaticDescription() const
{
	return TEXT( "Actualiza el siguiente punto en el recorrido" );
}