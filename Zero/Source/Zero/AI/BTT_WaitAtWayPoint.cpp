// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_WaitAtWayPoint.h"
#include "AIC_EnemyBase.h"
#include "Interface_Enemy.h"
#include "PatrolRoute.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_WaitAtWayPoint::UBTT_WaitAtWayPoint(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("WaitAtWayPoint");
	bNotifyTick = true;
}

void UBTT_WaitAtWayPoint::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	//if (bTaskCompleted) return;

	if (RemainingTime > 0)
	{
		
	}
	
	RemainingTime -= DeltaSeconds;

	if (RemainingTime <= 0 && PatrolRoute->WayPoints.Num() > 1)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "WAIT WAIPOINT END");

		//PatrolRoute->IncrementPatrolRoute();
		IInterface_Enemy::Execute_StopWayPointAnimation(OwnerComp.GetAIOwner());

		bTaskCompleted = true;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTT_WaitAtWayPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
//	bTaskCompleted = true;
	if (auto* const cont = Cast<AAIC_EnemyBase>(OwnerComponent.GetAIOwner()))
	{
		PatrolRoute = IInterface_Enemy::Execute_GetPatrolRoute(OwnerComponent.GetAIOwner());

		if (PatrolRoute)
		{
			int32 Index = PatrolRoute->PatrolIndex;

			RemainingTime = PatrolRoute->WayPoints[Index]->WaitTime;
			
			if (RemainingTime == 0.f)
			{
				//return EBTNodeResult::Succeeded;
			}
			else
			{
			}

			//bTaskCompleted = false;

			if (PatrolRoute->WayPoints[Index]->AnimToPlay)
			{
				IInterface_Enemy::Execute_PlayWayPointAnimation(OwnerComponent.GetAIOwner(), PatrolRoute->WayPoints[Index]->AnimToPlay);
			}

			
			PatrolRoute->IncrementPatrolRoute();
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "INCREMENT PATROL ROUTE -EXECUTE");

			return EBTNodeResult::InProgress;
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "PATROL ROUTE NULL");

			return EBTNodeResult::Succeeded;

		}
	}

	return EBTNodeResult::Failed;
}
