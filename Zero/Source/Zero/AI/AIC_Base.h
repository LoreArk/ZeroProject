// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BTT_Attack.h"
#include "AIModule.h"
#include "Navigation/PathFollowingComponent.h"
#include "Interface_AIAgent.h"
#include "AIC_Base.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EAIStates : uint8
{
	Passive		  UMETA(DisplayName = "Passive"),
	Combat		  UMETA(DisplayName = "Combat"),
	Frozen		  UMETA(DisplayName = "Frozen"),
	Wait		  UMETA(DisplayName = "Wait"),
	Investigating UMETA(DisplayName = "Investigating"),
	Seeking		  UMETA(DisplayName = "Seeking"),
	Mantle		  UMETA(DisplayName = "Mantle"),
	SendAlert     UMETA(DisplayName = "Send Alert"),
	Dead		  UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EDistanceState : uint8
{
	Far    UMETA(DisplayName = "Far"),
	Mid    UMETA(DisplayName = "Mid"),
	Close     UMETA(DisplayName = "Close")
};

USTRUCT(BlueprintType)
struct ZERO_API FAIAttackSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Setting")
	UAnimMontage* AnnounceMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Setting")
	EDistanceState TargetDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Setting")
	float MinAngleFromTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Setting")
	float MaxAngleFromTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Setting")
	float AcceptanceRadius;


	FAIAttackSettings()
		: AnnounceMontage(nullptr),
		TargetDistance(EDistanceState::Close),
		MinAngleFromTarget(0.0f),
		MaxAngleFromTarget(0.0f),
		AcceptanceRadius(0.0f)
	{
	}
};

class FOnAttackFinished;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackFinished, AAIC_Base*, Attacker);

UCLASS()
class ZERO_API AAIC_Base : public AAIController, public IInterface_AIAgent
{
	GENERATED_BODY()

public:
	//explicit AAIC_EnemyBase(FObjectInitializer const& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UPROPERTY()
	float DistanceFromTarget;


	UPROPERTY()
	EDistanceState CurrentDistanceState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* AttackTarget;

	UPROPERTY()
	AActor* LastAttackedTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	EAIStates AiState;

	UPROPERTY(BlueprintAssignable)
	FOnAttackFinished OnAttackFinishedDelegate;

	UPROPERTY()
	class UBTT_Attack* CurrentAttackTask = nullptr;

	virtual void Attack_Implementation() override;

	virtual float SetMovementSpeed_Implementation(EGait MovementSpeed) override;

	UFUNCTION()
	virtual void MoveToAttackTarget(AActor* Target, float AcceptanceRadius);

	virtual void OnAttackMoveCompleted(FAIRequestID, const FPathFollowingResult& Result);

	UFUNCTION()
	virtual void OnDeath();

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;

};
