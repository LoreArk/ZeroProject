// Fill out your copyright notice in the Description page of Project Settings.


#include "AIC_Base.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

void AAIC_Base::BeginPlay()
{
	Super::BeginPlay();

}

void AAIC_Base::Attack_Implementation()
{
	IInterface_AIAgent::Execute_Attack(GetPawn());
	LastAttackedTarget = AttackTarget;
}

float AAIC_Base::SetMovementSpeed_Implementation(EGait MovementSpeed)
{
	return IInterface_AIAgent::Execute_SetMovementSpeed(GetPawn(), MovementSpeed);
}

void AAIC_Base::MoveToAttackTarget(AActor* Target, float AcceptanceRadius)
{
	EPathFollowingRequestResult::Type MoveRequestResult = MoveToActor(Target, AcceptanceRadius);

	UE_LOG(LogTemp, Warning, TEXT("START MOVEMENT ATTACK"));
	// Bind to the OnRequestFinished delegate

	if (MoveRequestResult == EPathFollowingRequestResult::RequestSuccessful)
	{
		if (BrainComponent->IsActive() == false)
		{
			return;
		}
		GetPathFollowingComponent()->OnRequestFinished.AddUObject(
			this, &AAIC_Base::OnAttackMoveCompleted
		);
	}
	else if (MoveRequestResult == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		if (BrainComponent->IsActive() == false)
		{
			return;
		}
		UE_LOG(LogTemp, Warning, TEXT("START MOVEMENT ATTACK: ALREADY AT GOAL"));
		FPathFollowingResult MoveResult(EPathFollowingResult::Success);
		OnAttackMoveCompleted(FAIRequestID::CurrentRequest, MoveResult);
	}
	else if (MoveRequestResult == EPathFollowingRequestResult::Failed)
	{
		if (BrainComponent->IsActive() == false)
		{
			return;
		}
		UE_LOG(LogTemp, Warning, TEXT("START MOVEMENT ATTACK: FAILED"));
		FPathFollowingResult MoveResult(EPathFollowingResult::Aborted);
		OnAttackMoveCompleted(FAIRequestID::CurrentRequest, MoveResult);
	}
	else
	{
		if (BrainComponent->IsActive() == false)
		{
			return;
		}
		// Fallback for unexpected request results
		UE_LOG(LogTemp, Error, TEXT("START MOVEMENT ATTACK: UNHANDLED REQUEST RESULT!"));

		FPathFollowingResult MoveResult(EPathFollowingResult::Aborted);
		OnAttackMoveCompleted(FAIRequestID::CurrentRequest, MoveResult);
	}
}

void AAIC_Base::OnAttackMoveCompleted(FAIRequestID, const FPathFollowingResult& Result)
{
}

void AAIC_Base::OnDeath()
{
}

void AAIC_Base::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AAIC_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
