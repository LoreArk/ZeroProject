// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SetFocus.h"
#include "Interface_Enemy.h"
#include "AIC_EnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_SetFocus::UBTT_SetFocus(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Set Focus");
}

EBTNodeResult::Type UBTT_SetFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	UObject* Target = OwnerComponent.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey());
	if (Target)
	{
		AActor* Actor = Cast<AActor>(Target);
		if (Actor)
		{
			OwnerComponent.GetAIOwner()->SetFocus(Actor);
		}
	}
	else
	{
		FVector TargetLocation = OwnerComponent.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());

		OwnerComponent.GetAIOwner()->SetFocalPoint(TargetLocation);
	}
	
	

	return EBTNodeResult::Succeeded;
}
