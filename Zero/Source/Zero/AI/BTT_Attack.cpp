// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Attack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface_Enemy.h"
#include "AIC_Base.h"
#include "AIC_EnemyBase.h"
#include "EnemyCharacter.h"

UBTT_Attack::UBTT_Attack(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	CachedOwnerComponent = &OwnerComponent;
	
	AAIC_Base* AIC = Cast<AAIC_Base>(OwnerComponent.GetAIOwner());
	if (AIC)
	{
		//UE_LOG(LogTemp, Warning, TEXT("BIND ON ATTACK FINISHED DELEGATE"));
		//AIC->OnAttackFinishedDelegate.RemoveDynamic(this, &UBTT_Attack::OnAttackFinished); // Ensure not double-bound
		//AIC->OnAttackFinishedDelegate.AddDynamic(this, &UBTT_Attack::OnAttackFinished);
		//AIC->CurrentAttackTask = this;

		if (IInterface_AIAgent* const interface = Cast<IInterface_AIAgent>(OwnerComponent.GetAIOwner()))
		{
			if (GrabAttack == false)
			{
				UE_LOG(LogTemp, Warning, TEXT("BTT - Attack"));

				IInterface_AIAgent::Execute_Attack(OwnerComponent.GetAIOwner());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("BTT - Try Grab Attack"));
				IInterface_Enemy::Execute_TryGrabAttack(OwnerComponent.GetAIOwner()->GetPawn());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AIC COMPONENT NOT FOUND FOR ATTACK TASK"));
	}

	return EBTNodeResult::InProgress;
}

void UBTT_Attack::OnAttackFinished(AAIC_Base* AIC)
{
	UE_LOG(LogTemp, Error, TEXT(" --- FINISH ATTACK TASK"));
	if (AIC && CachedOwnerComponent)
	{
		if (CachedOwnerComponent)
		{
			//AIC->OnAttackFinishedDelegate.RemoveDynamic(this, &UBTT_Attack::OnAttackFinished);

			FinishLatentTask(*CachedOwnerComponent, EBTNodeResult::Succeeded);			//UE_LOG(LogTemp, Warning, TEXT("FINISH ATTACK TASK SUCCSSEDE"));
			UE_LOG(LogTemp, Error, TEXT("FINISH ATTACK TASK"));

		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, "FINISH TASK FAILED: owner component not found");

			UE_LOG(LogTemp, Error, TEXT("FINISH ATTACK TASK FAILED: owner component not found"));
		}
	}
}

