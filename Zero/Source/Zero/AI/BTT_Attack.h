// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_Attack.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API UBTT_Attack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	explicit UBTT_Attack(FObjectInitializer const& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;

	UFUNCTION()
	void OnAttackFinished(AAIC_Base* AIC);


	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
	bool GrabAttack;
};
