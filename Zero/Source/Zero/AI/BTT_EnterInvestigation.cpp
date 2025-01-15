// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_EnterInvestigation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface_Enemy.h"
#include "AIC_EnemyBase.h"

UBTT_EnterInvestigation::UBTT_EnterInvestigation(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Enter Investigation";
}

EBTNodeResult::Type UBTT_EnterInvestigation::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	AAIC_EnemyBase* AIC = Cast<AAIC_EnemyBase>(OwnerComponent.GetAIOwner());
	if (AIC)
	{
		if (IInterface_Enemy* const interface = Cast<IInterface_Enemy>(OwnerComponent.GetAIOwner()))
		{
			IInterface_Enemy::Execute_PassiveToInvestigation(OwnerComponent.GetAIOwner());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AIC COMPONENT NOT FOUND FOR ENTER INVESTIGATION TASK"));
	}
	return EBTNodeResult::Type();
}
