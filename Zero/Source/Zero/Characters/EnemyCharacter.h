// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PossessableCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/WidgetComponent.h"
#include "CombatManager.h"
#include "Weapon.h"
#include "Interface_Interactable.h"
#include "Interface_AIAgent.h"
#include "AIAreaOfAction.h"
#include "Interface_Enemy.h"
#include "AIC_EnemyBase.h"
#include "EnemyCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAIPossessed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBlockedEnter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBlockedExit);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAlertStart, FVector, AlertLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAlertEnd);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPOVReceivedStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPOVReceivedFinished);

UCLASS()
class ZERO_API AEnemyCharacter : public APossessableCharacter, public IInterface_Enemy, public IInterface_Interactable, public IInterface_AIAgent
{
	GENERATED_BODY()

public:

	AEnemyCharacter();

	UBehaviorTree* GetBehviorTree();

	virtual FRotator GetBaseAimRotation() const override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void Look(const FInputActionValue& Value) override;
	virtual void Move(const FInputActionValue& Value) override;

	void PossessionInputStart(const FInputActionValue& Value) override;
	void PossessionInputTriggered(const FInputActionInstance& Instance) override;
	void PossessionInputCompleted(const FInputActionValue& Value) override;

	void PossessAimTarget() override;

	virtual void PossessTargetAfterBlend(AActor* ActorToPossess) override;

	virtual void PlayHitReaction_Implementation(UAnimMontage* Montage) override;

	virtual void InteractTrace() override;

	virtual bool TakeDamage_Implementation(FDamageSettings DamageSettings, FHitResult HitResult, AActor* DamageDealer) override;

	virtual void OnDeathEvent(AActor* DeadActor) override;

	UPROPERTY()
	FTimerHandle DeathTimerHandle;

	UFUNCTION()
	void Revive();

	UPROPERTY()
	FOnPOVReceivedStarted OnPovReceivedStarted;

	UPROPERTY()
	FOnPOVReceivedFinished OnPOVReceivedFinished;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* MeshSpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FPCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* POVCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UComponent_ObjectPool* ObjectPoolComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* HeadDummyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AAIAreaOfAction* AreaOfAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Possession Settings")
	float PossessionAreaRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Possession Settings")
	float OutOfAreaMaxTime;

	UPROPERTY()
	AAIC_EnemyBase* AIC;

	UPROPERTY()
	FOnAlertEnd OnAlertEnd;

	UPROPERTY()
	FOnAlertStart OnAlertStart;

	UFUNCTION()
	void SetupPossession();

	UFUNCTION()
	void SetupPOV();

	UFUNCTION()
	void OnExitPOV();

	UFUNCTION()
	void SetupLeaveBody();

	UFUNCTION()
	void ReturnToPlayer(AActor* DamageDealer);

	UFUNCTION()
	void SetDummyHead(bool bIsFirstPerson);

	UFUNCTION()
	void InitDummyHead();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<UAnimMontage*, FAIAttackSettings> AttacksMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<UAnimMontage*, FAIGrabAttackSettings> GrabAttackMap;

	UPROPERTY(VisibleAnywhere)
	UAnimMontage* AttackToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* InvestigateMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* LookAroundMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* LeftBodyMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ReviveMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* StabVictimMontage;

	UPROPERTY()
	bool bCanAttack;

	UPROPERTY()
	bool bIsAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsBlocked;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bAttackAim;

	UPROPERTY(BlueprintCallable)
	FOnBlockedEnter OnBlockedEnter;

	UPROPERTY(BlueprintCallable)
	FOnBlockedExit OnBlockedExit;

	UPROPERTY()
	ACombatManager* CombatManager;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AWeapon> WeaponToEquip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	AWeapon* SpawnedWeapon;

	UPROPERTY(BlueprintAssignable)
	FOnAIPossessed OnAIPossessed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSightSettings PassiveSightSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSightSettings InvestigationSightSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSightSettings CombatSightSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSightSettings SeekingSightSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* AttackFlagWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* VulnerabilityFlag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* PossessionFlag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PossessedCapsuleRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StayDeadTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	EAlertBehavior AlertBehavior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punishing Attack")
	UAnimMontage* GrabMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punishing Attack")
	UAnimMontage* GrabAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punishing Attack")
	float PunishingAttackTargetRunThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punishing Attack")
	float PunishingAttackTargetCloseThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punishing Attack")
	float PunishingAttackBackFacingThreshold;

private:

	//UPROPERTY()
	//AActor* PlayerCharacterRef;

	UFUNCTION()
	void UpdateWidgetRotation(UWidgetComponent* TargetWidget);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReturnToPlayerAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Attack_1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* CurrentWayPointMontage;

	UFUNCTION()
	void ReturnToPlayerInputStarted(const FInputActionValue& Value);

	UPROPERTY()
	FVector PossessedStartLocation;

	UPROPERTY()
	FTimerHandle PossessionAreaLimitTimerHandle;

	UPROPERTY()
	FTimerHandle HitReactionCooldownTimerHandle;

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* Tree;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	APatrolRoute* PatrolRoute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	APatrolRoute* AlertPatrolRoute;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	FDamageSettings CurrentDamageSettings;

	UPROPERTY()
	TArray<AActor*> LastDamagedActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* EvasionFlagTarget;

	UPROPERTY(EditAnywhere)
	float EvasionTraceLenght = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FP Attacks", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Attack1Montage;

	UFUNCTION()
	virtual void Attack1Started();

	virtual void OnGameLoadedEvent() override;

public:

	//Enemy interface
	virtual float SetMovementSpeed_Implementation(EGait MovementSpeed) override;

	virtual APatrolRoute* GetPatrolRoute_Implementation() override;

	virtual AAIAreaOfAction* GetAreaOfAction_Implementation() override;

	virtual void Attack_Implementation() override;

	virtual void PassiveToInvestigation_Implementation() override;

	virtual void LookAround_Implementation() override;

	virtual void ToggleEvasionFlag_Implementation(bool bIsEnabled) override;

	virtual void AttackTraceStart_Implementation(FDamageSettings DamageSettings) override;

	virtual void AttackTraceTick_Implementation() override;

	virtual void AttackTraceEnd_Implementation() override;

	virtual void SetIsTarget_Implementation(bool bIsTarget, bool bCheckIsPossessable, bool bUsingPerception) override;

	virtual bool CanBePossessed_Implementation() override;

	virtual AEnemyCharacter* GetOwnerCharacter_Implementation() override;

	virtual void SetAttackFocus_Implementation() override;
	virtual void ClearAttackFocus_Implementation() override;

	virtual void ReviveAfterDeath_Implementation() override;

	virtual void SetPerceivedMaterial_Implementation(bool bEnable) override;

	virtual void PlayWayPointAnimation_Implementation(UAnimMontage* MontageToPlay) override;
	virtual void StopWayPointAnimation_Implementation() override;

			//Grab system
	virtual bool IsFacingAttackTargetBack_Implementation() override;
	virtual bool IsAttackTargetRunningClose_Implementation() override;
	virtual void TryGrabAttack_Implementation() override;
	virtual void DoGrabAttackDamage_Implementation(FDamageSettings DamageSettings) override;
	virtual void StopGrabAttack_Implementation() override;

	//virtual void PunishAttackTraceStart_Implementation() override;
	virtual void PunishAttackTraceTick_Implementation() override;
	virtual void PunishAttackTraceEnd_Implementation() override;

	//interaction interface

	virtual void Interact_Implementation(AActor* InteractingActor) override;
	virtual void EnterInteractionTargeting_Implementation() override;
	virtual void ExitInteractionTargeting_Implementation() override;

	//perception interface overrides
	virtual void SeenNewActor_Implementation(AActor* SeenActor) override;

	virtual UCameraComponent* GetActiveCamera() const;

	UFUNCTION(BlueprintNativeEvent)
	void AttackHitFX(FTransform HitTransform);
	void AttackHitFX_Implementation(FTransform HitTransform);

	UFUNCTION()
	void PlaySelectedAttackMontage();

	UFUNCTION()
	void EndAttack(UAnimMontage* Montage, bool bForceInterrupt);

	UFUNCTION()
	void SetSightSettings(EAIStates AiState);

	UFUNCTION()
	void OnCombatEnter(FVector ActorLocation);

	UFUNCTION()
	void EvasionFlagTrace();

	UFUNCTION()
	bool IsVulnerable();

	UFUNCTION()
	void SetVulnerabilityWidgetEnabled(bool bEnabled);

	UFUNCTION()
	void GrabAttack(AActor* Victim);

	UFUNCTION()
	void GrabAttackInterruption();

	UPROPERTY()
	AActor* GrabVictim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveSystem")
	FString UniqueID;
};
