#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "AIC_Ally.h"
#include "Interface_AIAgent.h"
#include "Interface_Ally.h"
#include "Component_ObjectPool.h"
#include "BehaviorTree/BehaviorTree.h"
#include "PlayerAllyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API APlayerAllyCharacter : public ABaseCharacter, public IInterface_AIAgent, public IInterface_Ally
{
	GENERATED_BODY()
	
public:

	APlayerAllyCharacter();

	UBehaviorTree* GetBehviorTree();

	UPROPERTY();
	AAIC_Ally* AIC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UComponent_ObjectPool* ObjectPoolComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<UAnimMontage*, FAIAttackSettings> AttacksMap;

	UPROPERTY(VisibleAnywhere)
	UAnimMontage* AttackToPlay;

	UPROPERTY()
	bool bIsAttacking;

	UFUNCTION()
	void PlaySelectedAttackMontage();

	UFUNCTION()
	void EndAttack(UAnimMontage* Montage, bool bForceInterrupt);

	virtual void AttackTraceStart_Implementation(FDamageSettings DamageSettings) override;

	virtual void AttackTraceTick_Implementation() override;

	virtual void AttackTraceEnd_Implementation() override;

	UPROPERTY()
	FDamageSettings CurrentDamageSettings;

	UPROPERTY()
	TArray<AActor*> LastDamagedActors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName TraceStartName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName TraceEndName;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	//AI Agent Interface

	virtual void Attack_Implementation() override;

	virtual float SetMovementSpeed_Implementation(EGait MovementSpeed) override;

	virtual void SetAttackFocus_Implementation() override;
	virtual void ClearAttackFocus_Implementation() override;

	//Ally Interface

	virtual void SetMoveToTaskLocation_Implementation(FVector Location) override;

	virtual void SetAttackTarget_Implementation(AActor* Target) override;

	virtual void StopMoveToTask_Implementation() override;

	virtual void OnDeathEvent(AActor* DeadActor) override;

	virtual void PlayHitReaction_Implementation(UAnimMontage* Montage);

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* Tree;

	UFUNCTION()
	void OnDamageReceived(AActor* Dealer);
};
