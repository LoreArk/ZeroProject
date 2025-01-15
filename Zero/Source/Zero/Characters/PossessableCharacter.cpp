// Fill out your copyright notice in the Description page of Project Settings.


#include "PossessableCharacter.h"
#include "ZeroGameMode.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "InputMappingContext.h"
#include "EnemyCharacter.h"
#include "Interface_Interactable.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Component_Damagable.h"
#include "Interface_Damagable.h"
#include "DamageStructs.h"
#include "StaticFunctions.h"
#include "Runtime/AnimGraphRuntime/Public/KismetAnimationLibrary.h"


APossessableCharacter::APossessableCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	PerceptionComponent = CreateDefaultSubobject<UComponent_Perception>(TEXT("Perception Component"));

	DefaultCapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	bShouldReceiveFallDamage = true;
}

void APossessableCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AController* MyController = GetController())
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(MyController))
		{
			if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
				{
					UE_LOG(LogTemp, Warning, TEXT("Adding mapping context in BeginPlay for %s"), *GetName());
					Subsystem->AddMappingContext(DefaultMappingContext, 0);
				}
			}
			PlayerController->PlayerCameraManager->ViewPitchMin = -90.0f;
			PlayerController->PlayerCameraManager->ViewPitchMax = 65.0f;
		}
	}
	
	OnGameLoaded.AddDynamic(this, &APossessableCharacter::OnGameLoadedEvent);
}

void APossessableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsPlayerControlled())
	{
		UpdateMovement();
		InteractTrace();
	}

	UpdateFallState();
}

void APossessableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		//Jumping
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &APossessableCharacter::Move);
		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &APossessableCharacter::Look);
		EnhancedInputComponent->BindAction(AimInputAction, ETriggerEvent::Triggered, this, &APossessableCharacter::Aim);
		EnhancedInputComponent->BindAction(AimInputAction, ETriggerEvent::Completed, this, &APossessableCharacter::AimCompleted);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &APossessableCharacter::Interact);

		EnhancedInputComponent->BindAction(PossessionAction, ETriggerEvent::Started, this, &APossessableCharacter::PossessionInputStart);
		EnhancedInputComponent->BindAction(PossessionAction, ETriggerEvent::Triggered, this, &APossessableCharacter::PossessionInputTriggered);
		EnhancedInputComponent->BindAction(PossessionAction, ETriggerEvent::Completed, this, &APossessableCharacter::PossessionInputCompleted);

		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &APossessableCharacter::PauseEvent);

	}
}

void APossessableCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	APlayerController* PlayerController = Cast<APlayerController>(NewController);

	if (PlayerController)
	{
		if (StatsHud.IsValid() == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("OnPossess: STATS HUD needs to be created for actor: %s"), *GetName());
			//CreateHUD(PlayerController);
			FTimerHandle CreateHUDTimerHandle;
			GetWorldTimerManager().SetTimer(CreateHUDTimerHandle, this, &APossessableCharacter::CreateHUD, 0.2f, false);
		}
		else if (StatsHud->IsVisible() == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("OnPossess: ENABLE HUD for actor: %s"), *GetName());
			//StatsHud->SetVisibility(ESlateVisibility::Visible);
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &APossessableCharacter::EnableHUD, 0.2f, false);
		}
	}

	if (StatsHud.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnPossess: STATS HUD already created"));
	}
	else
	{
		
	}
	
}

void APossessableCharacter::EnableHUD()
{
	StatsHud->SetVisibility(ESlateVisibility::Visible);
}

AZeroGameMode* APossessableCharacter::GetGameMode()
{
	AZeroGameMode* GM = IInterface_GameMode::Execute_GetZeroGameMode(UGameplayStatics::GetGameMode(GetWorld()));

	if (GM == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("PossessableCharacter GetGameMode - Zero Game Mode is null!"))
	}

	return GM;
}

void APossessableCharacter::OnGameLoadedEvent()
{
	if (StatsHud.IsValid() == false) return;

	StatsHud->UpdateHealthBar();
	StatsHud->UpdateManaBar();
}

void APossessableCharacter::UnPossessed()
{
	Super::UnPossessed();

	if (StatsHud.IsValid())
	{
		if (StatsHud->IsVisible())
		{
			UE_LOG(LogTemp, Warning, TEXT("UnPossess: HIDE HUD for actor: %s"), *GetName());
			StatsHud->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void APossessableCharacter::Move(const FInputActionValue& Value)
{
	if (CurrentHitReaction)
		return;
	if (bEnteredPOV)
		return;

	FVector2D MovementVector = Value.Get<FVector2D>();
	MovementVector2D = MovementVector;
	MovementVector2D.Normalize();

	if (!bWantsToJog)
	{
		bWantsToWalk = true;
	}

	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APossessableCharacter::Look(const FInputActionValue& Value)
{
	if (CurrentHitReaction)
		return;
	if (bEnteredPOV)
		return;

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APossessableCharacter::Aim(const FInputActionValue& Value)
{
	if (CurrentHitReaction)
	{
		bWantsToAim = false;
		return;
	}
	if (bEnteredPOV)
		return;

	bWantsToAim = true;
	bWantsToJog = false;
}

void APossessableCharacter::AimCompleted(const FInputActionValue& Value)
{
	bWantsToAim = false;
}

void APossessableCharacter::Interact()
{
	if (CurrentHitReaction)
		return;
	if (bEnteredPOV)
		return;
	if (bWantsToAim)
	{
		AimInteract();
		return;
	}
	AActor* Interactable = InteractionTarget == nullptr ? InteractionCandidate : InteractionTarget;

	if (Interactable)
	{
		IInterface_Interactable::Execute_Interact(Interactable, this);
		return;
	}

	Traversal(TraversalTrace());
}

void APossessableCharacter::InteractTrace()
{
	if (bEnteredPOV)
		return;
	if (CurrentHitReaction)
		return;

	FVector TraceStart = GetActorLocation();
	FVector CharacterForward = GetActorForwardVector() * 100.f;

	FVector TraceEnd = TraceStart + CharacterForward;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	FHitResult HitResult = StaticFunctions::SphereCollisionTraceObject(GetWorld(), TraceStart, TraceEnd, ObjectTypes, ActorsToIgnore, 1.f);

	bool bTargetVisible = true;

	if (AActor* HitActor = HitResult.GetActor())
	{
		if (HitActor->GetClass()->ImplementsInterface(UInterface_Interactable::StaticClass()))
		{
			if (HitActor != InteractionTarget && InteractionTarget != nullptr)
			{
			//	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "REMOVE INTERACT TARGET");
				IInterface_Interactable::Execute_ExitInteractionTargeting(InteractionTarget);
			}

			if (HitActor != InteractionTarget)
			{
				InteractionTarget = HitActor;
			//	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, "SET INTERACT TARGET");
				IInterface_Interactable::Execute_EnterInteractionTargeting(InteractionTarget);

			}
		}
		else
		{
			bTargetVisible = false;
		}
	}
	else
	{
		bTargetVisible = false;
	}

	if (bTargetVisible == false)
	{
		if (InteractionTarget != nullptr)
		{
			IInterface_Interactable::Execute_ExitInteractionTargeting(InteractionTarget);
			InteractionTarget = nullptr;
		}
	}
}

void APossessableCharacter::PauseEvent()
{
	OnGamePaused.Broadcast();
}

void APossessableCharacter::Traversal(FHitResult HitResult)
{
	if (bPerformingTraversal)
		return;

	FHitResult TraversalType = TraversalTrace();

	if (CurrentTraversalType == ETraversalType::Mantle && MantleMontage)
	{
		FVector DistanceMantleStart = StaticFunctions::GetLocationAtDistanceXFromActorB(this, HitResult.GetActor(), 100.f);
		//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation("MantleStart", DistanceMantleStart);
		FVector WarpingLocationEnd = HitResult.Location;
		WarpingLocationEnd.Z -= 8.f;
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation("MantleEnd", WarpingLocationEnd);

		bPerformingTraversal = true;
		GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveAll(this);
		GetMesh()->GetAnimInstance()->Montage_Play(MantleMontage);
		GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &APossessableCharacter::TraversalEnd);
	}
	if (CurrentTraversalType == ETraversalType::Vault && VaultMontage)
	{
		FVector WarpedVaultLocation = StaticFunctions::GetLocationAtDistanceXFromActorB(this, HitResult.GetActor(), 1.f);
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation("VaultStart", WarpedVaultLocation);

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		bPerformingTraversal = true;
		GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveDynamic(this, &APossessableCharacter::TraversalEnd);
		GetMesh()->GetAnimInstance()->Montage_Play(VaultMontage);
		GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &APossessableCharacter::TraversalEnd);
	}
}

void APossessableCharacter::TraversalEnd(UAnimMontage* Montage, bool bInterrupted)
{
	bPerformingTraversal = false;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

bool APossessableCharacter::TakeDamage_Implementation(FDamageSettings DamageSettings, FHitResult HitResult, AActor* DamageDealer)
{
	if (bPerformingTraversal)
		return false;

	bool WasDamaged = Super::TakeDamage_Implementation(DamageSettings, HitResult, DamageDealer);

	if (StatsHud.IsValid())
	{
		if (IsPlayerControlled())
			StatsHud->UpdateHealthBar();
	}

	return WasDamaged;
}

void APossessableCharacter::PlayHitReaction_Implementation(UAnimMontage* Montage)
{
	Super::PlayHitReaction_Implementation(Montage);

	if (bPerformingTraversal)
	{
		TraversalEnd(nullptr, true);
	}
}

FTransform APossessableCharacter::GetSightOriginTransform_Implementation()
{
	return GetMesh()->GetSocketTransform(SightSettings.OriginSocketName);
}

UComponent_Perception* APossessableCharacter::GetPerceptionComponent_Implementation()
{
	return PerceptionComponent;
}

void APossessableCharacter::PossessionInputStart(const FInputActionValue& Value)
{
	bShouldStartPossessTimer = true;
	UE_LOG(LogTemp, Error, TEXT("Possession input was  started"));
}

void APossessableCharacter::PossessionInputTriggered(const FInputActionInstance& Instance)
{
	if (CurrentHitReaction)
		return;
	if (bShouldStartPossessTimer == false)
	{
		PossessInputTimer = 0.0f;
		return;
	}
	if (AimTarget == nullptr && POVTarget == nullptr)
	{
		PossessInputTimer = 0.0f;
		return;
	}


	PossessInputTimer += GetWorld()->GetDeltaSeconds();
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, FString::Printf(TEXT("Hold Time: %.2f seconds"), PossessInputTimer));
	
	int32 Mana = 0;
	if (GetGameMode())
	{
		Mana = IInterface_GameMode::Execute_GetPlayerCurrentMana(GetGameMode());
	}

	if (Mana <= 0)
		return;

	if (PossessInputTimer > PossessInputTime)
	{
		UE_LOG(LogTemp, Error, TEXT("POSSESSABLE CHAR -> PossessAimTarget"));
	//	GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, FString::Printf(TEXT("POSSESS TIME PASSED"), PossessInputTimer));
		PossessInputTimer = 0;
		PossessAimTarget();
	}
}

void APossessableCharacter::PossessionInputCompleted(const FInputActionValue& Value)
{
	if (CurrentHitReaction)
		return;

	UE_LOG(LogTemp, Error, TEXT("POSSESSABLE CHAR -> PossessionInputCompleted"));
	
	if (PossessInputTimer > 0.0f)
	{
		if (bEnteredPOV == false)
		{
			POVAimTarget();
		}
		else
		{
			ExitPOV();
		}
	}
	PossessInputTimer = 0;
	bShouldStartPossessTimer = false;
}

void APossessableCharacter::PossessAimTarget()
{
	if (CurrentHitReaction)
		return;

	bShouldStartPossessTimer = false;
	
	AActor* ActorToPossess;

	ActorToPossess = POVTarget ? POVTarget : AimTarget;

	if (IInterface_Enemy::Execute_CanBePossessed(ActorToPossess) == false)
		return;

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	DisableInput(PlayerController);
	if (ActorToPossess && PlayerController)
	{
		if (bEnteredPOV && POVTarget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Possess From POV"));
			AEnemyCharacter* Enemy = IInterface_Enemy::Execute_GetOwnerCharacter(POVTarget);
			Enemy->POVCamera->SetActive(false);
			Enemy->FPCamera->SetActive(true);
			
			bEnteredPOV = false;
			POVTarget = nullptr;

			DisableInput(PlayerController);
			PlayerController->SetViewTarget(this);
			PlayerController->SetViewTargetWithBlend(ActorToPossess, 0.5f, EViewTargetBlendFunction::VTBlend_Linear, 1.0f, true);
			PossessionBlendTimer;
			GetWorldTimerManager().SetTimer(PossessionBlendTimer, [this, ActorToPossess]()
				{
					PossessTargetAfterBlend(ActorToPossess);

				}, 0.5f, false);
		}
		else
		{
			DisableInput(PlayerController);
			PossessTargetAfterBlend(ActorToPossess);
		}

		if (StatsHud.IsValid())
		{
			StatsHud->ToggleCrosshair(false);
		}
	}
}

void APossessableCharacter::PossessTargetAfterBlend(AActor* ActorToPossess)
{
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(ActorToPossess);
	if (Enemy && Controller)
	{
		UE_LOG(LogTemp, Warning, TEXT("Possess After blend"));
		//bPossessInputEnabled = false;
		bWantsToAim = false;
		bResettingCamera = true;

		Controller->Possess(Enemy);

		IInterface_Enemy::Execute_SetIsTarget(ActorToPossess, false, false, false);
		AimTarget = nullptr;

		if (GetGameMode())
		{
			IInterface_GameMode::Execute_ConsumePlayerMana(GetGameMode(), 1);
		}
	}
}

void APossessableCharacter::POVAimTarget()
{
	if (AimTarget)
	{
		if (IInterface_Enemy::Execute_CanBePossessed(AimTarget) == false)
			return;

		POVTarget = AimTarget;
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController && POVTarget)
		{
			bEnteredPOV = true;

			AEnemyCharacter* EnemyTarget = IInterface_Enemy::Execute_GetOwnerCharacter(POVTarget);
			if (EnemyTarget)
			{
				if (GetClass()->ImplementsInterface(UInterface_Enemy::StaticClass()))
				{
					AEnemyCharacter* ThisEnemy = IInterface_Enemy::Execute_GetOwnerCharacter(this);
					ThisEnemy->OnPOVReceivedFinished.Broadcast();
				}

				IInterface_Enemy::Execute_SetIsTarget(EnemyTarget, false, false, false);
				EnemyTarget->OnPovReceivedStarted.Broadcast();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("POV Enemy is null"));
			}

			OnPOVStart.Broadcast();
			
			PlayerController->SetViewTargetWithBlend(POVTarget, 0.5f, EViewTargetBlendFunction::VTBlend_EaseInOut, 1.f, true);

			/*if (StatsHud.IsValid() && StatsHud->IsInViewport())
			{
				StatsHud->RemoveFromParent();
			}*/
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerController or POVTarget is null"));
		}

		AimTarget = nullptr;
	}
}

void APossessableCharacter::ExitPOV()
{
	if (POVTarget)
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			bEnteredPOV = false;

			if (GetClass()->ImplementsInterface(UInterface_Enemy::StaticClass()))
			{
				AEnemyCharacter* ThisEnemy = IInterface_Enemy::Execute_GetOwnerCharacter(this);
				ThisEnemy->FPCamera->SetActive(true);
				ThisEnemy->SetDummyHead(true);
			}

			AEnemyCharacter* Enemy = IInterface_Enemy::Execute_GetOwnerCharacter(POVTarget);
			Enemy->OnPOVReceivedFinished.Broadcast();
			PlayerController->SetViewTarget(this);

			UE_LOG(LogTemp, Warning, TEXT("EXIT POV"));
			OnPOVExit.Broadcast();

			POVTarget = nullptr;
		}
	}
}

FHitResult APossessableCharacter::TraversalTrace()
{
	FRotator MeshRotation = GetMesh()->GetSocketRotation("root");
	FVector MeshRight = UKismetMathLibrary::GetRightVector(MeshRotation);
	FVector ForwardVector_Mantle = MeshRight * 50.f;
	FVector ForwardVector_Vault = MeshRight * 100.f;

	FVector HeightLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z +50);
	
	FVector TraceStart_Mantle = HeightLocation + ForwardVector_Mantle;
	FVector TraceEnd_Mantle = FVector(TraceStart_Mantle.X, TraceStart_Mantle.Y, TraceStart_Mantle.Z - 50);

	FVector TraceStart_Vault = HeightLocation + ForwardVector_Vault;
	FVector TraceEnd_Vault = FVector(TraceStart_Vault.X, TraceStart_Vault.Y, TraceStart_Vault.Z - 50);

	TArray<AActor*> ActorsToIgnore = TArray<AActor*>();

	ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2);

	FHitResult HitResult_Mantle = StaticFunctions::SphereCollisionTraceChannel(GetWorld(), TraceStart_Mantle, TraceEnd_Mantle, TraceTypeQuery, ActorsToIgnore, 10.f);
	FHitResult HitResult_Vault = StaticFunctions::SphereCollisionTraceChannel(GetWorld(), TraceStart_Vault, TraceEnd_Vault, TraceTypeQuery, ActorsToIgnore, 10.f);

	if (HitResult_Mantle.bBlockingHit && !HitResult_Vault.bBlockingHit)
	{
		VaultingActor = HitResult_Mantle.GetActor();
		CurrentTraversalType =  ETraversalType::Vault;
	}
	else if (HitResult_Mantle.bBlockingHit && HitResult_Vault.bBlockingHit)
	{
		VaultingActor = HitResult_Mantle.GetActor();
		CurrentTraversalType = ETraversalType::Mantle;
	}
	else
	{
		CurrentTraversalType = ETraversalType::None;
	}

	return HitResult_Mantle;
}

void APossessableCharacter::UpdateMovement()
{
	MovementState = GetDesiredGait();

	float MaxSpeed = CalculateMaxSpeed();

	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = MaxSpeed;

}

 void APossessableCharacter::UpdateFallState()
{
	bWasFalling = bIsFalling;

	if (GetCharacterMovement()->IsFalling() && !bIsFalling)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "FALL START");
		FallStartHeight = GetActorLocation().Z;
		GetCapsuleComponent()->SetCapsuleRadius(FallingCapsuleRadius);
	}

	bIsFalling = GetCharacterMovement()->IsFalling();
	if (!bIsFalling && bWasFalling && bShouldReceiveFallDamage)
	{
		DamagableComponent->FallDamage(FallStartHeight);
		GetCapsuleComponent()->SetCapsuleRadius(DefaultCapsuleRadius);

		if (StatsHud.IsValid() && StatsHud->IsVisible())
			StatsHud->UpdateHealthBar();
	}
}

void APossessableCharacter::AimTrace()
{
	if (bEnteredPOV)
		return;
	if (CurrentHitReaction)
		return;
	if (GetActiveCamera() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GetActiveCamera returned nullptr!"));
		return;
	}
	FVector TraceStart = GetActiveCamera()->GetComponentLocation();
	FVector CameraForward = GetActiveCamera()->GetForwardVector() * AimRange;

	FVector TraceEnd = TraceStart + CameraForward;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	FHitResult HitResult = StaticFunctions::SphereCollisionTraceObject(GetWorld(), TraceStart, TraceEnd, ObjectTypes, ActorsToIgnore, 1.f);

	bool bTargetVisible = true;

	if (AActor* HitActor = HitResult.GetActor())
	{
		if (Cast<AEnemyCharacter>(HitActor))
		{
			// Ensure AimTarget is valid
			if (AimTarget != nullptr && HitActor != AimTarget)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "REMOVE AIM TARGET");
				IInterface_Enemy::Execute_SetIsTarget(AimTarget, false, false, false);
			}

			if (HitActor != AimTarget)
			{
				AimTarget = HitActor;
				// Ensure DamagableComponent is valid before using it
				if (DamagableComponent != nullptr)
				{
					int32 PlayerMana = IInterface_GameMode::Execute_GetPlayerCurrentMana(UGameplayStatics::GetGameMode(GetWorld()));

					IInterface_Enemy::Execute_SetIsTarget(AimTarget, true, PlayerMana > 0, false);
					//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, "SET AIM TARGET");

					if (IsPlayerControlled() && GetGameMode() != nullptr)
					{
						GetGameMode()->AddMemorizedCharacter(AimTarget);
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("DamagableComponent is null!"));
				}
			}
		}
		else
		{
			bTargetVisible = false;
		}
	}
	else
	{
		bTargetVisible = false;
	}

	if (!bTargetVisible && AimTarget != nullptr)
	{
		IInterface_Enemy::Execute_SetIsTarget(AimTarget, false, false, false);
		AimTarget = nullptr;
	}
}

void APossessableCharacter::AimInteract_Implementation()
{
}

float APossessableCharacter::CalculateMaxSpeed()
{
	// Get the character's velocity and rotation
	FVector Velocity = GetCharacterMovement()->Velocity;
	FRotator Rotation = GetBaseAimRotation();
	Rotation.Roll = 0;
	Rotation.Pitch = 0;
	
	// Calculate direction based on velocity and rotation
	float Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Rotation);
	
	// Get the strafe speed from the curve
	if (StrafeSpeedMapCurve == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: STRAFE SPEED MAP CURVE NULL - assign in editor"), *GetName());
		StrafeSpeedMap = 1.0f;
	}
	else
	{
		StrafeSpeedMap = StrafeSpeedMapCurve->GetFloatValue(UKismetMathLibrary::Abs(Direction));
	}
	
	// Initialize the speed values based on the movement state
	FVector SpeedValuesVector = WalkSpeed; // Assuming WalkSpeed is defined as FVector

	// Determine speed values based on the current gait
	switch (MovementState)
	{
	case EGait::None:
	case EGait::Walk:
		SpeedValuesVector = WalkSpeed;
		break;
	case EGait::Jog:
		SpeedValuesVector = JogSpeed;
		break;
	case EGait::Sprint:
		SpeedValuesVector = SprintSpeed;
		break;
		break;
	case EGait::Aim:
		SpeedValuesVector = AimSpeed;
	default:
		break;
	}

	if (GetMovementComponent()->IsCrouching())
	{
		SpeedValuesVector = CrouchSpeed;
	}
	
	// Map the strafe speed to the appropriate range
	float ClampedRangeA = UKismetMathLibrary::MapRangeClamped(StrafeSpeedMap, 0.0f, 1.0f, SpeedValuesVector.X, SpeedValuesVector.Y);
	float ClampedRangeB = UKismetMathLibrary::MapRangeClamped(StrafeSpeedMap, 1.0f, 2.0f, SpeedValuesVector.Y, SpeedValuesVector.Z);

	// Return the appropriate range based on the strafe speed
	if (StrafeSpeedMap < 1.0f)
	{
		//UE_LOG(LogTemp, Warning, TEXT("ClampedRangeA: %f"), ClampedRangeA);
		return ClampedRangeA;
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT(" ClampedRangeB: %f"), ClampedRangeB);
		return ClampedRangeB;
	}
}

EGait APossessableCharacter::GetDesiredGait()
{
	EGait DesiredGait = EGait::Walk;

	if (bWantsToAim)
	{
		return DesiredGait = EGait::Aim;
	}
	if (bWantsToWalk)
	{
		return DesiredGait;
	}
	
	if (bWantsToJog && MovementVector2D.Y > 0.75f)
	{
		//UE_LOG(LogTemp, Warning, TEXT("WANTS TO JOG"));
		if (GetCharacterMovement()->IsCrouching())
		{
			UnCrouch();
		}
		return DesiredGait = EGait::Jog;
	}
	else if (bWantsToJog)
	{
		bWantsToJog = false;
	}

	return DesiredGait;
}

void APossessableCharacter::CreateHUD()
{
	if (AController* MyController = GetController())
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(MyController))
		{
			if (HudClass && PlayerController)
			{
				StatsHud = CreateWidget<UCharacterHUD>(PlayerController, HudClass);
				if (StatsHud.IsValid())
				{
					StatsHud->AddToViewport();
					StatsHud->InitHUD(this);
					StatsHud->SetVisibility(ESlateVisibility::Hidden);
					StatsHud->SetVisibility(ESlateVisibility::Visible);

					StatsHud->SetDamagableComponent(DamagableComponent);
					StatsHud->DamagableComponent = DamagableComponent;
					StatsHud->UpdateHealthBar();
					StatsHud->UpdateManaBar();
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Attempt created HUD: is not player controlled"));
		}
	}
	
}
