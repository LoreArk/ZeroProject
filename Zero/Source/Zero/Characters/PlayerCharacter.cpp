#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Interface_Sigil.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnemyCharacter.h"
#include "StaticFunctions.h"
#include "PlayerHUD.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "DummyPlayerCharacter.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 115.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 40.f));
	CameraBoom->SocketOffset = FVector(.0f, 40.f, .0f);
	CameraBoom->bEnableCameraLag = true;


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DefaultCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = true;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, -1.0f, 0.0f); 
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;

	WalkSpeed = FVector(200.f, 160.f, 130.f);
	JogSpeed = FVector(320.f, 0.f, 0.f);
	CrouchSpeed = FVector(150.f, 110.f, 110.f);
	AimSpeed = FVector(180.f, 140.f, 110.f);

	CameraTransitionSpeed = 8.0f;
	AimCameraSocketOffset = FVector(20.f, 62.f, -15.f);
	CrouchAimCameraSocketOffset = FVector(20.f, 62.f, 30.f);
	PerceptionCameraSocketOffset = FVector(145.0f, .0f, 90.f);

	InventoryComponent = CreateDefaultSubobject<UComponent_Inventory>(TEXT("InventoryComponent"));
	PlayerObjectPool = CreateDefaultSubobject<UComponent_PlayerObjectPool>(TEXT("Object Pool"));
	//DamagableComponent = CreateDefaultSubobject<UComponent_Damagable>(TEXT("Damagable Component"));


	bShouldMakeNoise = true;
}



void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultCameraSocketOffset = CameraBoom->SocketOffset;

	FActorSpawnParameters SpawnParams;

	if (DummyPlayerClass)
	{
		CreatedDummy = GetWorld()->SpawnActor<ADummyPlayerCharacter>(DummyPlayerClass, GetActorLocation(), GetActorRotation(), SpawnParams);
	}
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);


	APlayerController* PlayerController = Cast<APlayerController>(NewController);
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}

	UE_LOG(LogTemp, Warning, TEXT("Player Possessed"));
	bPossessInputEnabled = true;

	if (bEnteredPOV)
	{
		if (POVTarget)
		{
			bEnteredPOV = false;

			AEnemyCharacter* Enemy = IInterface_Enemy::Execute_GetOwnerCharacter(POVTarget);
			Enemy->OnPOVReceivedFinished.Broadcast();
		}
	}
}

void APlayerCharacter::UnPossessed()
{
	Super::UnPossessed();

	if (bEnteredPerception)
	{
		ExitPerception();
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCamera(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::TryCrouch);
		EnhancedInputComponent->BindAction(JogAction, ETriggerEvent::Started, this, &APlayerCharacter::Jog);
		EnhancedInputComponent->BindAction(CameraSideAction, ETriggerEvent::Started, this, &APlayerCharacter::ChangeCameraSideRequest);

		EnhancedInputComponent->BindAction(PerceptionAction, ETriggerEvent::Started, this, &APlayerCharacter::EnterPerception);
		EnhancedInputComponent->BindAction(PerceptionAction, ETriggerEvent::Triggered, this, &APlayerCharacter::PerceptionTrigger);
		EnhancedInputComponent->BindAction(PerceptionAction, ETriggerEvent::Completed, this, &APlayerCharacter::PerceptionCompleted);

		EnhancedInputComponent->BindAction(SigilInputAction, ETriggerEvent::Started, this, &APlayerCharacter::SigilAction);
		EnhancedInputComponent->BindAction(SigilMenuAction, ETriggerEvent::Started, this, &APlayerCharacter::SigilMenuActionStarted);
		EnhancedInputComponent->BindAction(ChangeSigilAction, ETriggerEvent::Started, this, &APlayerCharacter::ChangeSigilStarted);

		EnhancedInputComponent->BindAction(CastDummyCharacter, ETriggerEvent::Started, this, &APlayerCharacter::CastDummyCharacterStarted);

		EnhancedInputComponent->BindAction(EvasionAction, ETriggerEvent::Started, this, &APlayerCharacter::TryEvasion);
	}
}

void APlayerCharacter::Aim(const FInputActionValue& Value)
{
	if (bEnteredSigilMenu)
		return;

	if (bEnteredPerception)
		return;

	Super::Aim(Value);

	if (CurrentHitReaction || bPerformingTraversal)
	{
		bWantsToAim = false;
		return;
	}

	if (StatsHud.IsValid())
	{
		StatsHud->ToggleCrosshair(true);
	}

	AimTrace();
}

void APlayerCharacter::AimCompleted(const FInputActionValue& Value)
{
	if (bEnteredSigilMenu)
		return;

	if (bEnteredPerception)
		return;

	Super::AimCompleted(Value);

	ExitAim();
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (bEnteredSigilMenu)
		return;
	if (bEnteredPerception)
		return;

	Super::Move(Value);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	if (bEnteredSigilMenu)
		return;
	if (bPerformingStabAction)
		return;

	Super::Look(Value);
}

void APlayerCharacter::PlayHitReaction_Implementation(UAnimMontage* Montage)
{
	if (bEnteredSigilMenu)
	{
		ToggleSigilsMenu(false);
	}
	if(bWantsToAim)
		ExitAim();

	if(bEnteredPerception)
		ExitPerception();
	Super::PlayHitReaction_Implementation(Montage);
}

void APlayerCharacter::UpdateMovement()
{
	if (bEnteredSigilMenu)
		return;

	Super::UpdateMovement();

	if (GetCharacterMovement()->Velocity == FVector::ZeroVector || bPerformingTraversal || CurrentHitReaction)
	{
		bUseControllerRotationYaw = false;

		if (bWantsToJog)
		{
			bWantsToJog = false;
		}
	}
	else
	{
		bUseControllerRotationYaw = true;
	}
}

void APlayerCharacter::UpdateCamera(float DeltaTime)
{
	FVector TargetOffset = GetTargetCameraOffset();
	
	LerpCameraOffset(TargetOffset, DeltaTime, CameraTransitionSpeed);

	if (bResettingCamera && CameraBoom->SocketOffset.Equals(DefaultCameraSocketOffset, 1.0f))
	{
		bResettingCamera = false;
	}

	if (bIsChangingCameraSide)
	{
		float TransitionSpeed = (bWantsToAim || bResettingCamera) ? CameraTransitionSpeed / 2 : CameraTransitionSpeed;
		float NewYValue = FMath::FInterpTo(CameraBoom->SocketOffset.Y, TargetOffset.Y, DeltaTime, TransitionSpeed);

		CameraBoom->SocketOffset.Y = NewYValue;

		if (FMath::IsNearlyEqual(NewYValue, TargetOffset.Y, 1.0f))
		{
			bIsChangingCameraSide = false;
		}
	}

	/*
	float TargetYValue = DefaultCameraSocketOffset.Y;

	float UnscaledDelta = GetWorld()->GetDeltaSeconds() / UGameplayStatics::GetGlobalTimeDilation(GetWorld());
	if (bWantsToAim)
	{
		if (GetMovementComponent()->IsCrouching())
		{
			TargetYValue = CrouchAimCameraSocketOffset.Y;
			CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, CrouchAimCameraSocketOffset, UnscaledDelta, CameraTransitionSpeed);
		}
		else
		{
			TargetYValue = AimCameraSocketOffset.Y;
			CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, AimCameraSocketOffset, UnscaledDelta, CameraTransitionSpeed);
		}
	}
	else if (bResettingCamera && !bEnteredPerception)
	{
		CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, DefaultCameraSocketOffset, GetWorld()->GetDeltaSeconds(), CameraTransitionSpeed);

		if (CameraBoom->SocketOffset.Equals(DefaultCameraSocketOffset, 1.0f))
		{
			bResettingCamera = false;
		}
	}

	if (bIsChangingCameraSide)
	{
		FVector CurrentSocketOffset = CameraBoom->SocketOffset;
		float TransitionSpeed = CameraTransitionSpeed;

		if (bWantsToAim || bResettingCamera)
		{
			TransitionSpeed = CameraTransitionSpeed / 2;
		}

		float NewYValue = FMath::FInterpTo(CurrentSocketOffset.Y, TargetYValue, GetWorld()->GetDeltaSeconds(), TransitionSpeed);

		CameraBoom->SocketOffset = FVector(CurrentSocketOffset.X, NewYValue, CurrentSocketOffset.Z);

		if (FMath::IsNearlyEqual(NewYValue, TargetYValue, 1.0f)) 
		{
			bIsChangingCameraSide = false;
		}
	}*/
}

UCameraComponent* APlayerCharacter::GetActiveCamera() const
{
	return FollowCamera;
}


void APlayerCharacter::TryCrouch(const FInputActionValue& Value)
{
	if (bEnteredSigilMenu)
		return;
	if (bEnteredPerception)
		return;
	if (bEnteredPOV)
		return;

	if (GetMovementComponent()->IsCrouching())
	{
		UE_LOG(LogTemp, Warning, TEXT("UNCROUCH"), *GetName());
		UnCrouch();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Crouching"), *GetName());
		bWantsToJog = false;
		Crouch();
	}
}

void APlayerCharacter::Jog(const FInputActionValue& Value)
{
	if (bEnteredSigilMenu)
		return;
	if (bEnteredPerception)
		return;
	if (bEnteredPOV)
		return;

	bWantsToJog = !bWantsToJog;
	bWantsToWalk = !bWantsToJog;
}

void APlayerCharacter::ChangeCameraSideRequest(const FInputActionValue& Value)
{
	if (bEnteredSigilMenu)
		return;
	if (bEnteredPerception)
		return;

	if (bEnteredPOV)
		return;

	ChangeCameraSide();

}

void APlayerCharacter::EnterPerception(const FInputActionValue& Value)
{
	if (bEnteredSigilMenu)
		return;
	if (CurrentHitReaction)
		return;
	if (bEnteredPOV)
		return;

	bEnteredPerception = true;
	CameraBoom->bDoCollisionTest = false;

	GetGameMode()->SetHighlightMemorizedCharacters(true);
}

void APlayerCharacter::PerceptionTrigger(const FInputActionValue& Value)
{
	if (bEnteredSigilMenu)
		return;
	if (CurrentHitReaction)
		return;
	if (bEnteredPOV)
		return;
	if (bEnteredPerception == false)
		return;

	bWantsToAim = false;
	CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, PerceptionCameraSocketOffset, GetWorld()->GetDeltaSeconds(), 3.f);

	if(IsPlayerControlled())
		PerceptionTrace();
}

void APlayerCharacter::PerceptionCompleted(const FInputActionValue& Value)
{
	if (bEnteredSigilMenu)
		return;
	ExitPerception();
}

void APlayerCharacter::SigilMenuActionStarted(const FInputActionValue& Value)
{
	if (CurrentSigilMontage || CurrentHitReaction)
		return;

	if (bEnteredPerception)
		return;

	UE_LOG(LogTemp, Warning, TEXT("SIGIL MENU ACTIONSTARTED"));

	bEnteredSigilMenu = !bEnteredSigilMenu;

	bWantsToAim = false;
	ExitAim();

	ToggleSigilsMenu(bEnteredSigilMenu);
}

void APlayerCharacter::ChangeCameraSide()
{
	bCameraIsRight = !bCameraIsRight;

	CrouchAimCameraSocketOffset.Y = CrouchAimCameraSocketOffset.Y * -1;
	AimCameraSocketOffset.Y = AimCameraSocketOffset.Y * -1;
	DefaultCameraSocketOffset.Y = DefaultCameraSocketOffset.Y * -1;

	bIsChangingCameraSide = true;
}

void APlayerCharacter::ToggleSigilsMenu_Implementation(bool bEnabled)
{
	
}

void APlayerCharacter::StabActionEvent_Implementation(FStabActionSettings Settings, AActor* StabVictim)
{
	bPerformingStabAction = false;

	DamagableComponent->bIsInvincible = false;
}

void APlayerCharacter::CastGrabCounter_Implementation()
{
	if (SigilTarget)
	{
		IInterface_Enemy::Execute_StopGrabAttack(SigilTarget);
	}

	if (InventoryComponent->CurrentGrabCounter.Effect == ESigilAction::Fireball && SigilTarget != nullptr)
	{
		APooledActor* PooledFireball = PlayerObjectPool->SpawnActorFromPool(SigilTarget->GetActorTransform(), PlayerObjectPool->FireballActorPool);
		IInterface_Sigil::Execute_SetDamageSettings(PooledFireball, InventoryComponent->CurrentGrabCounter.DamageSettings, SigilTarget);
	}
	else if (InventoryComponent->CurrentGrabCounter.Effect == ESigilAction::Blocker && SigilTarget != nullptr)
	{
		APooledActor* PooledBlocker = PlayerObjectPool->SpawnActorFromPool(SigilTarget->GetActorTransform(), PlayerObjectPool->BlockerActorPool);
		IInterface_Sigil::Execute_SetDamageSettings(PooledBlocker, InventoryComponent->CurrentGrabCounter.DamageSettings, SigilTarget);
	}

	InventoryComponent->ConsumeGrabCounterSigil();
	GetMesh()->GetAnimInstance()->Montage_Play(CounterGrabMontage);
}


void APlayerCharacter::ExitPerception()
{
	bEnteredPerception = false;
	bResettingCamera = true;
	CameraBoom->bDoCollisionTest = true;
	GetGameMode()->SetHighlightMemorizedCharacters(false);

	if (AimTarget)
	{
		IInterface_Enemy::Execute_SetIsTarget(AimTarget, false, false, false);
		AimTarget = nullptr;
	}
}

void APlayerCharacter::PerceptionTrace()
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	FVector TraceStart = GetActiveCamera()->GetComponentLocation();
	FVector CameraForward = GetActiveCamera()->GetForwardVector() * AimRange;

	FVector TraceEnd = TraceStart + CameraForward;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	FHitResult HitResult = StaticFunctions::SphereCollisionTraceObject(GetWorld(), TraceStart, TraceEnd, ObjectTypes, ActorsToIgnore, 1.f);

	bool bTargetVisible = true;

	if (AActor* HitActor = HitResult.GetActor())
	{
		if (Cast<AEnemyCharacter>(HitActor) && GetGameMode()->MemorizedCharacters.Contains(HitActor))
		{
			if (HitActor != AimTarget && AimTarget != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "REMOVE AIM TARGET");

				IInterface_Enemy::Execute_SetIsTarget(AimTarget, false, false, true);
			}

			if (HitActor != AimTarget)
			{
				AimTarget = HitActor;
				IInterface_Enemy::Execute_SetIsTarget(AimTarget, true, DamagableComponent->Mana > 0, true);
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, "SET AIM TARGET");
			}
		}
	}
}

void APlayerCharacter::SigilAction(const FInputActionValue& Value)
{
	if (bGrabCounterFlag)
	{
		TryCastGrabCounter();
		return;
	}
	if (bEnteredSigilMenu)
		return;

	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(CurrentSigilMontage))
		return;

	switch (InventoryComponent->EquippedSigil.Type)
	{
	case ESigilActionType::None:

		break;
	case ESigilActionType::Self:
		CastSigil(this);
		break;
	case ESigilActionType::Mine:
		CastSigil(this);
		break;
	case ESigilActionType::Launch:
		if (AimTarget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Try Launch Spell"))
			if (AimTarget->GetClass()->ImplementsInterface(UInterface_Enemy::StaticClass()))
			{
				CastSigil(AimTarget);
			}
		}
		break;
	case ESigilActionType::Activation:
		if (bSigilPlaced)
		{
			bSigilPlaced = false;
			InventoryComponent->ConsumeEquippedSigil();
			OnSigilActivation.Broadcast();
		}
		else
		{
			CastSigil(this);
		}
		break;
	default:
		break;
	}
}


void APlayerCharacter::CastSigil(AActor* Target)
{
	UE_LOG(LogTemp, Warning, TEXT("CAST SIGIL"))

	CurrentSigilMontage = InventoryComponent->EquippedSigil.Montage;

	SigilTarget = Target;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageBlendingOut.RemoveDynamic(this, &APlayerCharacter::SigilActionEnd);
	}

	if (AnimInstance)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(CurrentSigilMontage);
		GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &APlayerCharacter::SigilActionEnd);
	}
}

void APlayerCharacter::SigilActionEnd(UAnimMontage* Montage, bool bInterrupted)
{
	CurrentSigilMontage = nullptr;
}

void APlayerCharacter::ChangeSigilStarted(const FInputActionValue& Value)
{
	if (bEnteredSigilMenu)
		return;
	if (bEnteredPerception)
		return;

	InventoryComponent->ChangeToNextSigil();
}

void APlayerCharacter::CastDummyCharacterStarted(const FInputActionValue& Value)
{
	if (bEnteredPerception || bEnteredPOV || bEnteredSigilMenu || CurrentHitReaction != nullptr)
		return;

	if (DamagableComponent->Mana <= 0)
		return;

	if (CreatedDummy)
	{
		CreatedDummy->DisableActor();
		GetCharacterMovement()->StopMovementImmediately();
		CreatedDummy->EnableActor(GetActorTransform());
		GetController()->Possess(CreatedDummy);
		IInterface_Player::Execute_ConsumeMana(this, 1);
	}
}

void APlayerCharacter::TryCastGrabCounter()
{
	if (InventoryComponent->CurrentGrabCounter.SigilName == "None")
		return;

	CastGrabCounter();
}

void APlayerCharacter::TryEvasion(const FInputActionValue& Value)
{
	if (bEvadeFlag && CurrentHitReaction == nullptr && bEvasionDisabled == false)
	{
		if (bIsReceivngGrab)
		{
			GetMesh()->GetAnimInstance()->Montage_Play(CounterGrabMontage);
			IInterface_Player::Execute_GrabAttackInterrupted(this);
		}
		SetEvasionEnabled(false, EvasionCooldownTime);
		DamagableComponent->bIsIntangible = true;
		GetMesh()->SetOverlayMaterial(InvincibilityMaterial);

		TWeakObjectPtr<APlayerCharacter> ThisWeak = this;
		GetWorldTimerManager().SetTimer(EvasionActivationTimerHandle, [ThisWeak]()
			{
				if (ThisWeak.IsValid())
				{
					ThisWeak->DamagableComponent->bIsIntangible = false;
					ThisWeak->GetMesh()->SetOverlayMaterial(nullptr);
				}
			}, EvasionActiveTime, false);
	}
}


void APlayerCharacter::CastSigilOfType_Implementation()
{
	if (InventoryComponent->EquippedSigil.Type == ESigilActionType::Launch)
	{
		if (InventoryComponent->EquippedSigil.Effect == ESigilAction::Fireball)
		{
			APooledActor* PooledFireball = PlayerObjectPool->SpawnActorFromPool(SigilTarget->GetActorTransform(), PlayerObjectPool->FireballActorPool);
			IInterface_Sigil::Execute_SetDamageSettings(PooledFireball, InventoryComponent->EquippedSigil.DamageSettings, SigilTarget);
		}
		else if (InventoryComponent->EquippedSigil.Effect == ESigilAction::Blocker)
		{
			APooledActor* PooledBlocker = PlayerObjectPool->SpawnActorFromPool(SigilTarget->GetActorTransform(), PlayerObjectPool->BlockerActorPool);
			IInterface_Sigil::Execute_SetDamageSettings(PooledBlocker, InventoryComponent->EquippedSigil.DamageSettings, SigilTarget);
		}
	}
	if (InventoryComponent->EquippedSigil.Effect == ESigilAction::CatSteps)
	{
		APooledActor* PooledCatSteps = PlayerObjectPool->SpawnActorFromPool(SigilTarget->GetActorTransform(), PlayerObjectPool->CatStepsActorPool);
	}
	if (InventoryComponent->EquippedSigil.Effect == ESigilAction::Explosive)
	{
		APooledActor* PooledExplosive = PlayerObjectPool->SpawnActorFromPool(SigilTarget->GetActorTransform(), PlayerObjectPool->ExplosiveActorPool);
		IInterface_Sigil::Execute_SetDamageSettings(PooledExplosive, InventoryComponent->EquippedSigil.DamageSettings, SigilTarget);
	}
	if (InventoryComponent->EquippedSigil.Type == ESigilActionType::Activation)
	{
		APooledActor* PooledExplosive = PlayerObjectPool->SpawnActorFromPool(SigilTarget->GetActorTransform(), PlayerObjectPool->SoundActorPool);
		bSigilPlaced = true;
		return;
	}

	InventoryComponent->ConsumeEquippedSigil();
}

void APlayerCharacter::SetInteractionTarget_Implementation(AActor* Target)
{
	if (Target == nullptr)
		UE_LOG(LogTemp, Warning, TEXT("Interaction target set to null"));
	InteractionCandidate = Target;
}

void APlayerCharacter::StartCatStepsEffect_Implementation(float Time)
{
	FTimerHandle TimerHandle;

	bShouldMakeNoise = false;
	bShouldReceiveFallDamage = false;

	GetWorldTimerManager().SetTimer(
		TimerHandle,
		this,
		&APlayerCharacter::EndCatStepsEffect,
		Time,
		false 
	);
}

void APlayerCharacter::POVAimTarget()
{
	Super::POVAimTarget();

	if (bEnteredPOV)
	{
		if (bEnteredPerception)
		{
			ExitPerception();
		}
	}
}

void APlayerCharacter::ToggleEvasionFlag_Implementation(bool bEnabled)
{
	bEvadeFlag = bEnabled;
}

bool APlayerCharacter::IsEvasionEnabled_Implementation()
{
	return !bEvasionDisabled;
}

void APlayerCharacter::AddMana_Implementation(int32 ManaAmount)
{
	DamagableComponent->Mana += ManaAmount;
	if (StatsHud.IsValid())
	{
		StatsHud->UpdateManaBar();
	}
}

void APlayerCharacter::ConsumeMana_Implementation(int32 Amount)
{
	DamagableComponent->Mana -= Amount;
	if (StatsHud.IsValid())
	{
		StatsHud->UpdateManaBar();
	}
}

void APlayerCharacter::StabAction_Implementation(AActor* Victim)
{
	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(StabMontage))
		return;
	
	if (bCameraIsRight)
	{
		ChangeCameraSide();
	}

	UnCrouch();
	FRotator WarpRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Victim->GetActorLocation());
	WarpRotation.Roll = 0;
	WarpRotation.Pitch = 0;
	GetController()->SetControlRotation(WarpRotation);

	FVector WarpLocation = StaticFunctions::GetLocationAtDistanceXFromActorB(this, Victim, 120.f);
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation("Victim", WarpLocation, WarpRotation);

	DamagableComponent->bIsInvincible = true;

	bUseControllerRotationYaw = false;
	bPerformingStabAction = true;
	StabActionEvent(StabActionsSettings[0], Victim);

	//GetMesh()->GetAnimInstance()->Montage_Play(StabMontage);
}



void APlayerCharacter::ReceiveGrabAttack_Implementation(AActor* EnemyActor)
{
	UnCrouch();
	FRotator WarpRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EnemyActor->GetActorLocation());
	//WarpRotation.Roll = 0;
	WarpRotation.Pitch = 0;
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation("grab", GetActorLocation(), WarpRotation);

	DamagableComponent->OnDamage.Broadcast(EnemyActor);

	bIsReceivngGrab = true;
	if(GetController())
		GetController()->SetControlRotation(WarpRotation);
	SetActorRotation(WarpRotation);

	bUseControllerRotationYaw = false;
	DamagableComponent->bIsInvincible = true;

	GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveAll(this);

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	BlendingOutDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
		{
			GrabFinished();
		});

	GetMesh()->GetAnimInstance()->Montage_Play(ReceiveGrabMontage);
	GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendingOutDelegate, ReceiveGrabMontage);

	InventoryComponent->SetGrabCounter();
	SigilTarget = EnemyActor;
}

void APlayerCharacter::GrabAttackInterrupted_Implementation()
{
	if (bIsReceivngGrab && ReceiveGrabMontage)
	{
		GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveAll(this);
		GetMesh()->GetAnimInstance()->Montage_Stop(0.15f, ReceiveGrabMontage);
		GrabFinished();
	}
}

void APlayerCharacter::GrabFinished()
{
	DamagableComponent->bIsInvincible = false;
	SigilTarget = nullptr;
	bIsReceivngGrab = false;
	FRotator rot = GetActorRotation();
	rot.Pitch = 0;
	rot.Roll = 0;
	SetActorRotation(rot);
}

void APlayerCharacter::SetGrabSigilEnabled_Implementation(bool bEnable)
{
	bGrabCounterFlag = bEnable;

	if (bEnable == false)
	{
		SigilTarget = nullptr;
	}
}

void APlayerCharacter::ShrineInteraction_Implementation(int32 ManaToAdd)
{

}

bool APlayerCharacter::TakeDamage_Implementation(FDamageSettings DamageSettings, FHitResult HitResult, AActor* DamageDealer)
{
	bool bWasDamaged = Super::TakeDamage_Implementation(DamageSettings, HitResult, DamageDealer);
	
	return bWasDamaged;
}

void APlayerCharacter::OnDeathEvent(AActor* Actor)
{
	GetMesh()->GetAnimInstance()->Montage_Play(DamagableComponent->DeathMontage);
	APlayerController* pc = Cast<APlayerController>(GetController());
	DisableInput(pc);
}

void APlayerCharacter::ForcedSetTransform_Implementation(FTransform NewTransform)
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);

	SetActorTransform(NewTransform, false, nullptr, ETeleportType::ResetPhysics);
	GetController()->SetControlRotation(NewTransform.Rotator());


	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
}

void APlayerCharacter::LerpCameraOffset(const FVector& TargetOffset, float DeltaTime, float TransitionSpeed)
{
	float UnscaledDelta = DeltaTime / UGameplayStatics::GetGlobalTimeDilation(GetWorld());
	CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, TargetOffset, UnscaledDelta, TransitionSpeed);
}

FVector APlayerCharacter::GetTargetCameraOffset() const
{
	if (bWantsToAim)
	{
		return GetMovementComponent()->IsCrouching() ? CrouchAimCameraSocketOffset : AimCameraSocketOffset;
	}
	if (bResettingCamera && !bEnteredPerception)
	{
		return DefaultCameraSocketOffset;
	}

	return DefaultCameraSocketOffset;
}

void APlayerCharacter::SetEvasionEnabled(bool bEnabled, float TimeRemaining)
{
	bEvasionDisabled = !bEnabled;

	if (StatsHud.IsValid())
	{
		if (UPlayerHUD* PlayerHUD = Cast<UPlayerHUD>(StatsHud))
		{
			PlayerHUD->SetEvadeIconEnabled(bEnabled);
		}
	}

	if (!bEnabled)
	{
		TWeakObjectPtr<APlayerCharacter> ThisWeak = this;
		GetWorld()->GetTimerManager().SetTimer(
			EvasionCoolDownTimerHandle,
			[ThisWeak]() {

				if (ThisWeak.IsValid())
				{
					ThisWeak->SetEvasionEnabled(true, 0.0f);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Evasion cooldown timer callback called for invalid object"));
				}
			},
			TimeRemaining,
			false
		);
	}
}

void APlayerCharacter::EndCatStepsEffect()
{
	bShouldMakeNoise = true;
	bShouldReceiveFallDamage = true;
}

void APlayerCharacter::PossessionInputStart(const FInputActionValue& Value)
{
	Super::PossessionInputStart(Value);
}

void APlayerCharacter::PossessionInputTriggered(const FInputActionInstance& Instance)
{
	Super::PossessionInputTriggered(Instance);
}

void APlayerCharacter::PossessionInputCompleted(const FInputActionValue& Value)
{
	Super::PossessionInputCompleted(Value);
}

void APlayerCharacter::ExitAim()
{
	if (AimTarget != nullptr)
	{
		IInterface_Enemy::Execute_SetIsTarget(AimTarget, false, false, false);
	}

	AimTarget = nullptr;
	bResettingCamera = true;

	if (StatsHud.IsValid())
	{
		StatsHud->ToggleCrosshair(false);
	}
}

void APlayerCharacter::OnGameLoadedEvent()
{
	APlayerController* pc = Cast<APlayerController>(GetController());
	EnableInput(pc);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);

	if (bEvasionDisabled)
	{
		SetEvasionEnabled(false, EvasionCooldownRemainingTime);
	}
	else
	{
		SetEvasionEnabled(true, 0.0f);
	}

	Super::OnGameLoadedEvent();
}

