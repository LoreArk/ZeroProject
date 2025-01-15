// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputAction.h"
#include "Component_ObjectPool.h"
#include "Component_Perception.h"
#include "Component_Perceivable.h"
#include "Curves/CurveFloat.h"
#include "Camera/CameraComponent.h" 
#include "Component_Damagable.h"
#include "CharacterHUD.h"
#include "ZeroGameMode.h"
#include "PossessableCharacter.generated.h"


UENUM(BlueprintType)
enum class ETraversalType : uint8
{
	None    UMETA(DisplayName = "None"),
	Mantle  UMETA(DisplayName = "Mantle"),
	Vault   UMETA(DisplayName = "Vault")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerPossessed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPOVStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPOVExit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameLoaded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGamePause);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLanded);

UCLASS()
class ZERO_API APossessableCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	APossessableCharacter();
	TWeakObjectPtr<UCharacterHUD> StatsHud;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HudClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* StrafeSpeedMapCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector2D MovementVector2D;

	UPROPERTY()
	FOnGamePause OnGamePaused;

	UPROPERTY()
	FOnGameLoaded OnGameLoaded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* MantleMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* VaultMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPerformingTraversal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UComponent_Perception* PerceptionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSightSettings SightSettings;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerPossessed OnPlayerPossessed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PossessInputTime;

	UPROPERTY()
	FOnPOVStart OnPOVStart;

	UPROPERTY()
	FOnPOVExit OnPOVExit;

	UPROPERTY(VisibleAnywhere)
	AActor* InteractionTarget;

	UPROPERTY(VisibleAnywhere)
	AActor* InteractionCandidate;

	UFUNCTION(BlueprintNativeEvent)
	void AimInteract();
	void AimInteract_Implementation();

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float FallingCapsuleRadius;

	UPROPERTY()
	float PossessInputTimer;

	UFUNCTION()
	float CalculateMaxSpeed();

	UFUNCTION()
	EGait GetDesiredGait();

	UFUNCTION()
	void CreateHUD();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void Move(const FInputActionValue& Value);
	virtual void Look(const FInputActionValue& Value);

	virtual void Aim(const FInputActionValue& Value);
	virtual void AimCompleted(const FInputActionValue& Value);

	virtual void Interact();

	virtual void Traversal(FHitResult HitResult);

	virtual UCameraComponent* GetActiveCamera() const { return nullptr; }

	virtual bool TakeDamage_Implementation(FDamageSettings DamageSettings, FHitResult HitResult, AActor* DamageDealer) override;
	virtual void PlayHitReaction_Implementation(UAnimMontage* Montage) override;

	//Perception Interface
	virtual FTransform GetSightOriginTransform_Implementation() override;
	virtual UComponent_Perception* GetPerceptionComponent_Implementation() override;

	UFUNCTION()
	virtual void PossessionInputStart(const FInputActionValue& Value);

	UFUNCTION()
	virtual void PossessionInputTriggered(const FInputActionInstance& Instance);

	UFUNCTION()
	virtual void PossessionInputCompleted(const FInputActionValue& Value);

	UFUNCTION()
	virtual void PossessAimTarget();

	UFUNCTION()
	virtual void PossessTargetAfterBlend(AActor* ActorToPossess);

	UFUNCTION()
	virtual void POVAimTarget();

	UFUNCTION()
	virtual void ExitPOV();

	UFUNCTION()
	FHitResult TraversalTrace();

	UFUNCTION()
	void TraversalEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	virtual void UpdateMovement();

	UFUNCTION()
	 void UpdateFallState();

	UFUNCTION()
	virtual void AimTrace();

	UFUNCTION()
	virtual void InteractTrace();

	UFUNCTION()
	void PauseEvent();

	UFUNCTION()
	void EnableHUD();

	UFUNCTION()
	virtual AZeroGameMode* GetGameMode();

	UPROPERTY()
	bool bShouldStartPossessTimer;

	UPROPERTY()
	bool bWantsToWalk;

	UPROPERTY()
	bool bWantsToJog;

	UPROPERTY()
	bool bWantsToAim;

	UPROPERTY()
	bool bWasFalling;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bIsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldReceiveFallDamage;

	UPROPERTY()
	float FallStartHeight;

	UPROPERTY()
	bool bResettingCamera;

	UPROPERTY()
	bool bPossessInputEnabled;

	UPROPERTY()
	bool bEnteredPOV;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AimInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PossessionAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PauseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Settings")
	AActor* AimTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Settings")
	float AimRange;

	UPROPERTY()
	AActor* POVTarget;

	UPROPERTY()
	float StrafeSpeedMap;

	UPROPERTY()
	AActor* VaultingActor;

	UPROPERTY()
	ETraversalType CurrentTraversalType;


	UPROPERTY()
	float DefaultCapsuleRadius;


	UPROPERTY()
	FTimerHandle PossessionBlendTimer;


	UFUNCTION()
	virtual void OnGameLoadedEvent();

};
