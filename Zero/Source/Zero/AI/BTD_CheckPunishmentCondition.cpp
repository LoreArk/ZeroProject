// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_CheckPunishmentCondition.h"
#include "Interface_Enemy.h"
#include "AIController.h"

bool UBTD_CheckPunishmentCondition::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    auto* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return false;

    auto* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn) return false;

    // Check if the pawn implements the interface
    if (ControlledPawn->GetClass()->ImplementsInterface(UInterface_Enemy::StaticClass()))
    {
        bool bIsPlayerClose = IInterface_Enemy::Execute_IsAttackTargetRunningClose(ControlledPawn);
        bool bIsFacingBack = IInterface_Enemy::Execute_IsFacingAttackTargetBack(ControlledPawn);

        return bIsPlayerClose || bIsFacingBack; 
    }

    return false;
}
