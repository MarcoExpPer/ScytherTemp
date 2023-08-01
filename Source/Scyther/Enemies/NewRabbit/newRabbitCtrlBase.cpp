// Fill out your copyright notice in the Description page of Project Settings.


#include "newRabbitCtrlBase.h"
#include "newRabbitPawnBase.h"

AnewRabbitCtrlBase::AnewRabbitCtrlBase()
{

}

void AnewRabbitCtrlBase::BeginPlay()
{
	Super::BeginPlay();

	rabbitPawn = Cast<AnewRabbitPawnBase>( this->GetPawn() );

}




EPathFollowingRequestResult::Type AnewRabbitCtrlBase::moveToPosition( FVector targetPosition )
{
	return EPathFollowingRequestResult::Failed;
}
