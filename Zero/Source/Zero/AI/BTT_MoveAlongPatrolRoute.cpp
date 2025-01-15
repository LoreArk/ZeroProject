// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MoveAlongPatrolRoute.h"
#include "AIC_EnemyBase.h"
#include "Interface_Enemy.h"
#include "PatrolRoute.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_MoveAlongPatrolRoute::UBTT_MoveAlongPatrolRoute(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("MoveAlongPatrolRoute");
}

EBTNodeResult::Type UBTT_MoveAlongPatrolRoute::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	if (auto* const cont = Cast<AAIC_EnemyBase>(OwnerComponent.GetAIOwner()))
	{
		APatrolRoute* PatrolRoute = IInterface_Enemy::Execute_GetPatrolRoute(OwnerComponent.GetAIOwner());
		
		if (PatrolRoute)
		{
			FVector TargetLocation = PatrolRoute->GetWayPointLocation();

			OwnerComponent.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), TargetLocation);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "PATROL ROUTE NULL");
		}
		FinishLatentTask(OwnerComponent, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
