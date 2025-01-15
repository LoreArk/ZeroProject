// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIModule.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Interface_Enemy.h"
#include "AIC_Base.h"
#include "AIC_EnemyBase.generated.h"

class UInterface_Enemy;



UENUM(BlueprintType)
enum class EAlertBehavior : uint8
{
	Patrol UMETA(DisplayName = "Patrol"),
	Seek	UMETA(DisplayName = "Seek")
};




USTRUCT(BlueprintType)
struct ZERO_API FAIGrabAttackSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Setting")
	UAnimMontage* GrabPairedAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Setting")
	float MinAngleFromTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Setting")
	float MaxAngleFromTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Setting")
	float AcceptanceRadius;

	FAIGrabAttackSettings()
		: GrabPairedAttack(nullptr),
		MinAngleFromTarget(0.0f),
		MaxAngleFromTarget(0.0f),
		AcceptanceRadius(0.0f)
	{}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackTargetSeen, AActor*, SeenActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatStart, FVector, TargetLocation);

UCLASS()
class ZERO_API AAIC_EnemyBase : public AAIC_Base, public IInterface_Enemy
{
	GENERATED_BODY()
	

public:
	//explicit AAIC_EnemyBase(FObjectInitializer const& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UPROPERTY()
	FVector StartLocation;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	AActor* PlayerRef;

	UPROPERTY()
	class AEnemyCharacter* CharacterRef;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	EAIStates PreviousAiState;

	FTimerHandle PerceptionTimerHandle;

	FTimerHandle InvestigationTimerHandle;

	UPROPERTY(BlueprintAssignable)
	FOnAttackTargetSeen OnTargetSeen;

	UPROPERTY(BlueprintAssignable)
	FOnCombatStart OnCombatStart;

	UPROPERTY(VisibleAnywhere)
	AActor* NewAttackTarget;

	UPROPERTY()
	bool bAlertMode;

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;

public:


	virtual APatrolRoute* GetPatrolRoute_Implementation() override;

	virtual AAIAreaOfAction* GetAreaOfAction_Implementation() override;

	virtual void PassiveToInvestigation_Implementation() override;

	virtual void SetStateAsPassive_Implementation() override;

	virtual void LookAround_Implementation() override;

	virtual bool CanReachTargetStraight_Implementation() override;

	virtual bool CanSeeTarget_Implementation() override;

	virtual void PlayWayPointAnimation_Implementation(UAnimMontage* MontageToPlay) override;
	virtual void StopWayPointAnimation_Implementation() override;


	UPROPERTY()
	TArray<AActor*> ActorsDamagedBy;

	UFUNCTION()
	virtual void UpdateDistanceState();

	UFUNCTION()
	virtual EDistanceState GetDistanceState();

	UFUNCTION()
	void HandleSensedSight(AActor* ActorSensed);

	UFUNCTION()
	bool IsLocationReachable(FVector TargetLocation);

	UFUNCTION()
	void HandleLostSight(AActor* LostActor);

	UFUNCTION()
	void HandleHearEvent(FNoiseStimulus NoiseSettings);

	UFUNCTION()
	void HandleSensedDamage(AActor* DamageDealer);

	UFUNCTION()
	void HandleAttackTargetDeath(AActor* DeadActor);

	UFUNCTION()
	AActor* GetBestAttackTarget();

	UFUNCTION()
	int32 GetPriorityForTarget(ETeam Team, AActor* TargetActor);

	UFUNCTION()
	void HandleSensedAlert(FVector AlertLocation);

	UFUNCTION()
	void HandleAlertEnd();

	UFUNCTION()
	void PassiveToInvestigationTransition(FVector Location);

	UFUNCTION()
	void EndInvestigationTransition();

	UFUNCTION()
	void SetStateAsCombat(bool bUseLastKnownAttackTarget, AActor* Target);

	UFUNCTION()
	void SetStateAsInvestigating(FVector Location);

	UFUNCTION()
	void SetStateAsSeeking(FVector Location);

	UFUNCTION()
	void SetStateAsDead();

	UFUNCTION()
	void SetBlockedState();

	UFUNCTION()
	void SetStateAsPreviousState();

	virtual void OnAttackMoveCompleted(FAIRequestID, const FPathFollowingResult& Result) override;

	void InterruptAttackMove();

	UFUNCTION()
	void PauseAI();

	UFUNCTION()
	void ResumeAI();

	virtual void OnDeath() override;

	UFUNCTION()
	void OnRevive();

	UFUNCTION()
	void OnGameLoadedStateSetup();

private:

	UPROPERTY()
	FVector InvestigationLocation;
};
