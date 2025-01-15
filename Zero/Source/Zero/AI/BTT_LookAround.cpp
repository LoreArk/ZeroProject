// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_LookAround.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface_Enemy.h"
#include "AIC_EnemyBase.h"

UBTT_LookAround::UBTT_LookAround(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Look Around";
}

EBTNodeResult::Type UBTT_LookAround::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{

	IInterface_Enemy::Execute_LookAround(OwnerComponent.GetAIOwner());

	return EBTNodeResult::Succeeded;
}
