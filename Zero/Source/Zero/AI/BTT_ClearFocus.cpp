// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ClearFocus.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIC_Base.h"

UBTT_ClearFocus::UBTT_ClearFocus(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Clear Focus");
}

EBTNodeResult::Type UBTT_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	//IInterface_Enemy::Execute_GetOwnerCharacter(OwnerComponent.GetAIOwner())

	if (AAIC_Base* AIC = Cast<AAIC_Base>(OwnerComponent.GetAIOwner()))
	{
		OwnerComponent.GetAIOwner()->ClearFocus(EAIFocusPriority::Move);
		OwnerComponent.GetAIOwner()->ClearFocus(EAIFocusPriority::Default);
		OwnerComponent.GetAIOwner()->ClearFocus(EAIFocusPriority::Gameplay);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CLEAR FOCUS TASK - AIC NOT VALID"));
	}

	return EBTNodeResult::Succeeded;
}
