// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_WormSoundShake.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Scyther/Enemies/Worm/WormPawn.h"


UBTT_WormSoundShake::UBTT_WormSoundShake()
{
	//SoundShake.AddObjectFilter( this, GET_MEMBER_NAME_CHECKED( UBTT_WormSoundShake, SoundShake ) ,);
}

//Si se llama a esta tarea, devuelve un Succeeded, Failed o InProgress
EBTNodeResult::Type UBTT_WormSoundShake::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{

	//Reproducimos e
	//Sounds of cracking wood

	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
	bb->SetValueAsBool( "isMoving?" , false);

	UFMODEvent* ctrl = Cast<UFMODEvent>(bb->GetValueAsObject(SoundShake.SelectedKeyName));

	UFMODBlueprintStatics::PlayEventAtLocation( GetWorld(), ctrl, OwnerComp.GetOwner()->GetTransform(), true );

	//Finalmente retornamos In progress 
	return EBTNodeResult::Succeeded;

}

//Permite añadir una descripción a esta tarea. Este texto se verá en el B.T
FString UBTT_WormSoundShake::GetStaticDescription() const
{
	return TEXT( "Actualiza el siguiente punto en el recorrido" );
}