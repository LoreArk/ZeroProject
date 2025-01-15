// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SetStateAsPassive.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface_Enemy.h"
#include "AIC_EnemyBase.h"

UBTT_SetStateAsPassive::UBTT_SetStateAsPassive(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Set State As Passive";
}

EBTNodeResult::Type UBTT_SetStateAsPassive::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{

	IInterface_AIAgent::Execute_SetStateAsPassive(OwnerComponent.GetAIOwner());

	return EBTNodeResult::Succeeded;
}
