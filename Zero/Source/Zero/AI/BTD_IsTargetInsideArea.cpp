// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_IsTargetInsideArea.h"
#include "AIC_EnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIAreaOfAction.h"

UBTD_IsTargetInsideArea::UBTD_IsTargetInsideArea(const FObjectInitializer& ObjectInitializer)
{

	NodeName = "Is Target Inside Area";
}

bool UBTD_IsTargetInsideArea::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (auto* const cont = Cast<AAIC_EnemyBase>(OwnerComp.GetAIOwner()))
	{
		if (AAIAreaOfAction* Area = IInterface_Enemy::Execute_GetAreaOfAction(OwnerComp.GetAIOwner()))
		{
			UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

			if (!BlackboardComp)
			{
				return true;
			}
			UObject* Object = BlackboardComp->GetValueAsObject(BlackboardKeyTargetActor.SelectedKeyName);
			AActor* TargetActor = Cast<AActor>(Object);

			return Area->IsLocationInsideBox(TargetActor->GetActorLocation());
		}
	}

	return true;
}
