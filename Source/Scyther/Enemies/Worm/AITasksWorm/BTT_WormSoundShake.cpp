// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_WormSoundShake.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Scyther/Enemies/Worm/AIWormCppController.h"
#include "Scyther/Enemies/Worm/WormPawn.h"


UBTT_WormSoundShake::UBTT_WormSoundShake()
{
	//SoundShake.AddObjectFilter( this, GET_MEMBER_NAME_CHECKED( UBTT_WormSoundShake, SoundShake ) ,);
}

//Si se llama a esta tarea, devuelve un Succeeded, Failed o InProgress
EBTNodeResult::Type UBTT_WormSoundShake::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{

	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
	if( bb->GetValueAsBool( "isMoving?" ) == true )
	{
		Cast<AAIWormCppController>(OwnerComp.GetOwner())->PlayChaseSound(bb->GetValueAsBool("AttackType"));
		bb->SetValueAsBool( "isMoving?", false );
	}
	//Finalmente retornamos In progress 
	return EBTNodeResult::Succeeded;

}

//Permite añadir una descripción a esta tarea. Este texto se verá en el B.T
FString UBTT_WormSoundShake::GetStaticDescription() const
{
	return TEXT( "Reproduce el sonido de persecucion del ataque elegido" );
}