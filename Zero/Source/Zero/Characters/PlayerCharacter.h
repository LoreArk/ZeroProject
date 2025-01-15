// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component_PlayerObjectPool.h"
#include "Interface_Player.h"
#include "Component_Inventory.h"
#include "PossessableCharacter.h"
#include "CameraAnimationSequence.h"
//#include <LevelSequenceActor.h>
//#include <LevelSequencePlayer.h>
#include <MovieSceneSequencePlayer.h>
#include "PlayerCharacter.generated.h"

class ADummyPlayerCharacter;
/**
 * 
 */

USTRUCT(BlueprintType)
struct FStabActionSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Moves Settings")
	UAnimMontage* StabMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Animation")
	TSoftObjectPtr<UCameraAnimationSequence> CameraAnimation;

	FStabActionSettings()
		: StabMontage(nullptr), CameraAnimation(nullptr)
	{
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSigilActivation);

UCLASS()
class ZERO_API APlayerCharacter : public APossessableCharacter, public IInterface_Player
{
	GENERATED_BODY()

public:

	APlayerCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	UComponent_Inventory* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	UComponent_PlayerObjectPool* PlayerObjectPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	FVector AimCameraSocketOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	FVector CrouchAimCameraSocketOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	FVector PerceptionCameraSocketOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	FVector GrabVictimCameraSocketOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float GrabVictimArmLenght;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraTransitionSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float PerceptionFOV;

	UPROPERTY()
	TArray<AActor*> MemorizedEnemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* SigilTarget;

	UPROPERTY(BlueprintAssignable)
	FOnSigilActivation OnSigilActivation;

	UPROPERTY(BlueprintReadWrite)
	bool bEnteredSigilMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldMakeNoise;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sigil Fireball")
	TSubclassOf<ADummyPlayerCharacter> DummyPlayerClass;

	UPROPERTY()
	ADummyPlayerCharacter* CreatedDummy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMaterialInterface* InvincibilityMaterial;

	UPROPERTY()
	bool bEvasionDisabled;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Evasion")
	float EvasionCooldownTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Evasion")
	float EvasionCooldownRemainingTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Evasion")
	float EvasionActiveTime;

	UPROPERTY()
	FTimerHandle EvasionCoolDownTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stab Action Settings")
	TArray<FStabActionSettings> StabActionsSettings;

	UFUNCTION(BlueprintNativeEvent)
	void ToggleSigilsMenu(bool bEnabled);
	void ToggleSigilsMenu_Implementation(bool bEnabled);

	UFUNCTION(BlueprintNativeEvent)
	void StabActionEvent(FStabActionSettings Settings, AActor* StabVictim);
	void StabActionEvent_Implementation(FStabActionSettings Settings, AActor* StabVictim);

	UFUNCTION(BlueprintNativeEvent)
	void CastGrabCounter();
	void CastGrabCounter_Implementation();


	UPROPERTY()
	bool bPerformingStabAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsReceivngGrab;

protected:

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Aim(const FInputActionValue& Value) override;
	virtual void AimCompleted(const FInputActionValue& Value) override;

	virtual void Move(const FInputActionValue& Value) override;

	virtual void Look(const FInputActionValue& Value) override;

	virtual void PlayHitReaction_Implementation(UAnimMontage* Montage) override;

	virtual void UpdateMovement() override;

	virtual void UpdateCamera(float DeltaTime);

	virtual void CastSigilOfType_Implementation() override;

	virtual void SetInteractionTarget_Implementation(AActor* Target) override;

	virtual void StartCatStepsEffect_Implementation(float Time) override;

	virtual void POVAimTarget() override;

	virtual void ToggleEvasionFlag_Implementation(bool bEnabled) override;

	virtual bool IsEvasionEnabled_Implementation() override;

	virtual void AddMana_Implementation(int32 ManaAmount) override;

	virtual void ConsumeMana_Implementation(int32 Amount) override;
	
	virtual void StabAction_Implementation(AActor* Victim) override;

	virtual void ReceiveGrabAttack_Implementation(AActor* EnemyActor) override;

	virtual void GrabAttackInterrupted_Implementation() override;

	void GrabFinished();

	virtual void SetGrabSigilEnabled_Implementation(bool bEnable) override;

	virtual void ShrineInteraction_Implementation(int32 ManaToAdd) override;

	virtual bool TakeDamage_Implementation(FDamageSettings DamageSettings, FHitResult HitResult, AActor* DamageDealer) override;

	virtual void OnDeathEvent(AActor* Actor) override;

	virtual void ForcedSetTransform_Implementation(FTransform NewTransform) override;

	UFUNCTION()
	void LerpCameraOffset(const FVector& TargetOffset,  float DeltaTime, float TransitionSpeed);

	UFUNCTION()
	FVector GetTargetCameraOffset() const;

	UFUNCTION()
	virtual void SetEvasionEnabled(bool bEnabled, float TimeRemaining);

	UFUNCTION()
	void EndCatStepsEffect();

	virtual UCameraComponent* GetActiveCamera() const;

	void PossessionInputStart(const FInputActionValue& Value) override;
	void PossessionInputTriggered(const FInputActionInstance& Instance) override;
	void PossessionInputCompleted(const FInputActionValue& Value) override;

	UFUNCTION()
	void ExitAim();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CastDummyCharacter;

	virtual void OnGameLoadedEvent() override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JogAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CameraSideAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PerceptionAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SigilInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SigilMenuAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeSigilAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* EvasionAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montages", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* StabMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montages", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReceiveGrabMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montages", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* CounterGrabMontage;

	UPROPERTY()
	UAnimMontage* CurrentSigilMontage;

	UPROPERTY()
	FVector DefaultCameraSocketOffset;
	
	UPROPERTY()
	bool bCameraIsRight;

	UPROPERTY()
	bool bIsChangingCameraSide;

	UPROPERTY()
	bool bEnteredPerception;

	UFUNCTION()
	void TryCrouch(const FInputActionValue& Value);

	UFUNCTION()
	void Jog(const FInputActionValue& Value);

	UFUNCTION()
	void ChangeCameraSideRequest(const FInputActionValue& Value);

	UFUNCTION()
	void EnterPerception(const FInputActionValue& Value);

	UFUNCTION()
	void PerceptionTrigger(const FInputActionValue& Value);

	UFUNCTION()
	void PerceptionCompleted(const FInputActionValue& Value);

	UFUNCTION()
	void SigilMenuActionStarted(const FInputActionValue& Value);

	UFUNCTION()
	void ChangeCameraSide();

	UFUNCTION()
	void ExitPerception();

	UFUNCTION()
	void PerceptionTrace();

	UFUNCTION()
	void SigilAction(const FInputActionValue& Value);

	UFUNCTION()
	void CastSigil(AActor* Target);

	UFUNCTION()
	void SigilActionEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void ChangeSigilStarted(const FInputActionValue& Value);

	UFUNCTION()
	void CastDummyCharacterStarted(const FInputActionValue& Value);

	UFUNCTION()
	void TryCastGrabCounter();


	UFUNCTION()
	void TryEvasion(const FInputActionValue& Value);

	UPROPERTY()
	bool bSigilPlaced;

	UPROPERTY()
	bool bEvadeFlag;

	UPROPERTY()
	FTimerHandle EvasionActivationTimerHandle;


	UPROPERTY()
	bool bGrabCounterFlag;

	UPROPERTY()
	AActor* GrabInstigator;



};
