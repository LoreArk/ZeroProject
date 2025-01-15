// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "PlayerCharacter.h"
#include "AIC_EnemyBase.h"
#include "Weapon.h"
#include "BTT_Attack.h"
#include "StaticFunctions.h"
#include "GameFramework/SpringArmComponent.h"
#include "ZeroGameMode.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"


AEnemyCharacter::AEnemyCharacter()
{
	MeshSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Mesh Spring Arm"));
	MeshSpringArm->SetupAttachment(RootComponent);
	MeshSpringArm->TargetArmLength = 0.0f;
	MeshSpringArm->SetRelativeLocation(FVector(4.312, -0.199, 52.746));

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Spring Arm"));
	CameraSpringArm->SetupAttachment(MeshSpringArm);
	CameraSpringArm->TargetArmLength = 0.0f;

	FPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FPCamera->SetupAttachment(CameraSpringArm);
	FPCamera->bAutoActivate = false;

	POVCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("POV Camera"));
	POVCamera->SetupAttachment(CameraSpringArm);
	POVCamera->bAutoActivate = false;

	FPCamera->SetRelativeLocation(FVector(15.822, 0.0f, 24.1));
	POVCamera->SetRelativeLocation(FVector(15.822, 0.0f, 24.1));
	
	PossessionFlag = CreateDefaultSubobject<UWidgetComponent>(TEXT("Possession Flag Widget"));
	AttackFlagWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Evasion Flag Widget"));
	VulnerabilityFlag = CreateDefaultSubobject<UWidgetComponent>(TEXT("Vulnerability Flag Widget"));

	PossessionFlag->SetVisibility(false);
	AttackFlagWidget->SetVisibility(false);
	VulnerabilityFlag->SetVisibility(false);

	PossessionFlag->SetupAttachment(RootComponent);
	AttackFlagWidget->SetupAttachment(RootComponent);
	VulnerabilityFlag->SetupAttachment(RootComponent);

	PossessionFlag->SetRelativeLocation(FVector(10.f, .0f, 111.f));
	AttackFlagWidget->SetRelativeLocation(FVector(35.f, .0f, 90.f));
	VulnerabilityFlag->SetRelativeLocation(FVector(4.37f, .0f, 41.14f));

	ObjectPoolComponent = CreateDefaultSubobject<UComponent_ObjectPool>(TEXT("Object Pool Component"));

	HeadDummyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Dummy Head Mesh"));
	HeadDummyMesh->SetupAttachment(RootComponent);

}

UBehaviorTree* AEnemyCharacter::GetBehviorTree()
{
	return Tree;
}

FRotator AEnemyCharacter::GetBaseAimRotation() const
{
	AController* CharController = GetController();
	if (CharController)
	{
		FRotator ControlRotation = CharController->GetControlRotation();

		if (AIC)
		{
			if (AIC->AttackTarget)
			{
				FVector PawnLocation = GetActorLocation();
				FVector TargetLocation = AIC->AttackTarget->GetActorLocation();
				FRotator LookAtRotation = (TargetLocation - PawnLocation).Rotation();

				// Update the pitch of the control rotation to look at the target
				ControlRotation.Pitch = LookAtRotation.Pitch;
			}
		}


		return ControlRotation;
	}

	return Super::GetBaseAimRotation();
}


void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UniqueID.IsEmpty())
	{
		AZeroGameMode* GameMode = Cast<AZeroGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			UniqueID = GameMode->GenerateUniqueEnemyID();
			GameMode->RegisterEnemyID(UniqueID, this);
		}
	}

	AIC = Cast<AAIC_EnemyBase>(GetController());

	if (AIC == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "AIC not found!");
	}

	InitDummyHead();

	//PlayerCharacterRef = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());
	PerceptionComponent->OwnerActor = this;
	PerceptionComponent->SelfPerceivable = PerceivableComponent;
	PerceptionComponent->CurrentSightSettings = PassiveSightSettings;
	if (AIC)
	{
		PerceptionComponent->OnHearEvent.AddDynamic(AIC, &AAIC_EnemyBase::HandleHearEvent);
		AIC->OnCombatStart.AddDynamic(this, &AEnemyCharacter::OnCombatEnter);
		OnAlertStart.AddDynamic(AIC, &AAIC_EnemyBase::HandleSensedAlert);
		OnAlertEnd.AddDynamic(AIC, &AAIC_EnemyBase::HandleAlertEnd);
	}
	else 
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "AIC not found!");
	}

	AZeroGameMode* GameMode = Cast<AZeroGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode != nullptr)
	{
		CombatManager = GameMode->CombatManager;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		if (WeaponToEquip)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();


			FTransform SocketTransform = FTransform();

			SpawnedWeapon = World->SpawnActor<AWeapon>(WeaponToEquip, SocketTransform, SpawnParams);

			if (SpawnedWeapon)
			{
				SpawnedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SpawnedWeapon->CarrySocketName);

			}
		}
	}

	POVCamera->SetActive(false);
	FPCamera->SetActive(false);

	POVCamera->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "pov_camera");
	OnPovReceivedStarted.AddDynamic(this, &AEnemyCharacter::SetupPOV);
	OnPOVReceivedFinished.AddDynamic(this, &AEnemyCharacter::OnExitPOV);

	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;
	bCanAttack = true;
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PossessionFlag->IsVisible())
	{
		UpdateWidgetRotation(PossessionFlag);
	}
	if (VulnerabilityFlag->IsVisible())
	{
		UpdateWidgetRotation(VulnerabilityFlag);
	}

	if (IsPlayerControlled())
	{
		AimTrace();

		float distanceFromPossessedLocation = FVector::Distance(GetActorLocation(), PossessedStartLocation);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), PossessedStartLocation, 50.0f, 12, FColor::Green, 0.0f, 1.0f);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), PossessedStartLocation, PossessionAreaRadius, 12, FColor::Red, 0.0f, 1.0f);


		if (distanceFromPossessedLocation > PossessionAreaRadius && !GetWorldTimerManager().IsTimerActive(PossessionAreaLimitTimerHandle))
		{
			UE_LOG(LogTemp, Warning, TEXT("CHAR OUTSIDE AREA"));
			TWeakObjectPtr<AEnemyCharacter> ThisWeak = this;
			GetWorldTimerManager().SetTimer(PossessionAreaLimitTimerHandle, [ThisWeak]()
				{
					if (ThisWeak.IsValid())
					{
						ThisWeak->ReturnToPlayer(nullptr);
					}
				}, OutOfAreaMaxTime, false);
		}
		else if (distanceFromPossessedLocation < PossessionAreaRadius && GetWorldTimerManager().IsTimerActive(PossessionAreaLimitTimerHandle))
		{
			UE_LOG(LogTemp, Warning, TEXT("CHAR RETURNED IN AREA"));
			GetWorldTimerManager().ClearTimer(PossessionAreaLimitTimerHandle);
		}
	}
	else if (PossessionAreaLimitTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(PossessionAreaLimitTimerHandle);
	}
}

void AEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, "POSSESSED");
	APlayerController* PlayerController = Cast<APlayerController>(NewController);
	AAIC_EnemyBase* EnemyAIController = Cast<AAIC_EnemyBase>(NewController);
	if (PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("ENEMY POSSESSED BY PLAYER")); 
		//bPossessInputEnabled = false;
		OnPlayerPossessed.Broadcast();
		SetupPossession();
		return;
	}
	else if (EnemyAIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("ENEMY POSSESSED BY AI"));

		OnAIPossessed.Broadcast();
	}
}

void AEnemyCharacter::UnPossessed()
{
	Super::UnPossessed();
}

void AEnemyCharacter::Look(const FInputActionValue& Value)
{
	if (CurrentHitReaction)
		return;
	if (bEnteredPOV)
		return;

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	FQuat DeltaRotation = FQuat(FVector::UpVector, FMath::DegreesToRadians(LookAxisVector.X)); 
	//MeshSpringArm->AddLocalRotation(DeltaRotation);
	AddControllerYawInput(LookAxisVector.X);

	//FirstPersonPitch += LookAxisVector.Y;

	FirstPersonPitch += LookAxisVector.Y;
	FirstPersonPitch = FMath::Clamp(FirstPersonPitch, -80.0f, 80.0f);

	if (FirstPersonPitch > -80.0f && FirstPersonPitch < 80.0f)
	{
		LookAxisVector.Y *= -1;
		FRotator NewRotation(0.0f, 0.0f, 0.0f); 
		NewRotation.Pitch = LookAxisVector.Y;

		CameraSpringArm->AddLocalRotation(NewRotation);
	}
}

void AEnemyCharacter::Move(const FInputActionValue& Value)
{
	if (CurrentHitReaction)
		return;
	if (bEnteredPOV)
		return;

	MovementVector2D = Value.Get<FVector2D>();

	FRotator YawRotation(0.0f, FPCamera->GetComponentRotation().Yaw, 0.0f);

	FVector WorldDirectionX = UKismetMathLibrary::GetRightVector(YawRotation);
	FVector WorldDirectionY = UKismetMathLibrary::GetForwardVector(YawRotation);

	AddMovementInput(WorldDirectionX, MovementVector2D.X, false);
	AddMovementInput(WorldDirectionY, MovementVector2D.Y, false);
}

void AEnemyCharacter::PossessionInputStart(const FInputActionValue& Value)
{
	if (CurrentHitReaction)
		return;
	Super::PossessionInputStart(Value);

}

void AEnemyCharacter::PossessionInputTriggered(const FInputActionInstance& Instance)
{
	if (CurrentHitReaction)
		return;
	Super::PossessionInputTriggered(Instance);
}

void AEnemyCharacter::PossessionInputCompleted(const FInputActionValue& Value)
{
	if (CurrentHitReaction)
		return;
	Super::PossessionInputCompleted(Value);
}

void AEnemyCharacter::PossessAimTarget()
{
	Super::PossessAimTarget();
	
}

void AEnemyCharacter::PossessTargetAfterBlend(AActor* ActorToPossess)
{
	Super::PossessTargetAfterBlend(ActorToPossess);
	if (GetWorldTimerManager().IsTimerActive(PossessionAreaLimitTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(PossessionAreaLimitTimerHandle);
	}
	UE_LOG(LogTemp, Error, TEXT("ENEMY PossessAimTarget -> SetupLeaveBody"));
	SetupLeaveBody();
}

void AEnemyCharacter::PlayHitReaction_Implementation(UAnimMontage* Montage)
{
	if (GrabVictim)
	{
		GrabAttackInterruption();
	}

	EndAttack(nullptr, true);
	AIC->InterruptAttackMove();

	Super::PlayHitReaction_Implementation(Montage);

	
	TWeakObjectPtr<AEnemyCharacter> ThisWeak = this;
	bCanAttack = false;
	GetWorldTimerManager().SetTimer(HitReactionCooldownTimerHandle, [ThisWeak]()
		{
			if (ThisWeak.IsValid())
			{
				ThisWeak->bCanAttack = true;
			}
		}, 1.f, false);
}

void AEnemyCharacter::InteractTrace()
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
		if (HitActor->Implements<UInterface_Enemy>())
			return;
		if (HitActor->GetClass()->ImplementsInterface(UInterface_Interactable::StaticClass()))
		{
			if (HitActor != InteractionTarget && InteractionTarget != nullptr)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "REMOVE INTERACT TARGET");
				IInterface_Interactable::Execute_ExitInteractionTargeting(InteractionTarget);
			}

			if (HitActor != InteractionTarget)
			{
				InteractionTarget = HitActor;
				//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, "SET INTERACT TARGET");
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

bool AEnemyCharacter::TakeDamage_Implementation(FDamageSettings DamageSettings, FHitResult HitResult, AActor* DamageDealer)
{
	bool bWasDamaged = Super::TakeDamage_Implementation(DamageSettings, HitResult, DamageDealer);


	return bWasDamaged;
}



void AEnemyCharacter::OnDeathEvent(AActor* DeadActor)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Death event");
	if (IsPlayerControlled())
	{
		GetWorldTimerManager().ClearTimer(PossessionAreaLimitTimerHandle);
		ReturnToPlayer(nullptr);
	}

	if (bIsBlocked)
	{
		OnBlockedExit.Broadcast();
	}

	PerceivableComponent->Team = ETeam::Enemies;

	AIC->OnDeath();
	
	if (bIsAttacking)
	{
		if (CombatManager)
		{
			CombatManager->RemoveAttackingActor(this);
		}
		bIsAttacking = false;
		AIC->OnAttackFinishedDelegate.Broadcast(AIC);
	}

	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(StabVictimMontage) == false)
	{
		GetMesh()->GetAnimInstance()->StopAllMontages(0.2f);
		if (DamagableComponent->DeathMontage)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Death montage");
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Death montage NULLL");
		}
		GetMesh()->GetAnimInstance()->Montage_Play(DamagableComponent->DeathMontage);
	}

	GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &AEnemyCharacter::Revive, StayDeadTime, false);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
}

void AEnemyCharacter::Revive()
{
	DamagableComponent->bIsInvincible = false;
	GetMesh()->GetAnimInstance()->Montage_Play(ReviveMontage);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AEnemyCharacter::ReviveAfterDeath_Implementation()
{
	DamagableComponent->Health = DamagableComponent->MaxHealth;
	AIC->OnRevive();
}

void AEnemyCharacter::SetPerceivedMaterial_Implementation(bool bEnable)
{

}

void AEnemyCharacter::PlayWayPointAnimation_Implementation(UAnimMontage* MontageToPlay)
{
	CurrentWayPointMontage = MontageToPlay;
	GetMesh()->GetAnimInstance()->Montage_Play(CurrentWayPointMontage);
}

void AEnemyCharacter::StopWayPointAnimation_Implementation()
{
	GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, CurrentWayPointMontage);
}

bool AEnemyCharacter::IsFacingAttackTargetBack_Implementation()
{
	if (AIC->AttackTarget)
	{
		if (IInterface_BaseCharacter::Execute_GetIsCrouching(AIC->AttackTarget))
			return false;
		// Get the vectors
		FVector EnemyForward = GetActorForwardVector().GetSafeNormal();        // Enemy's forward direction
		FVector PlayerBackward = -(AIC->AttackTarget->GetActorForwardVector().GetSafeNormal()); // Player's backward direction

		// Calculate the dot product
		float DotProductResult = FVector::DotProduct(EnemyForward, PlayerBackward);

		// Log for debugging
		//UE_LOG(LogTemp, Warning, TEXT("Dot Product Result (Facing Back Check): %f"), DotProductResult);
		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT("Dot Product Result: %f"), DotProductResult));
		}

		return PunishingAttackBackFacingThreshold > DotProductResult; 
	}
	return false;
}

bool AEnemyCharacter::IsAttackTargetRunningClose_Implementation()
{
	if (AIC->AttackTarget)
	{
		FVector PlayerVelocity = AIC->AttackTarget->GetVelocity();
		return PlayerVelocity.Size() > PunishingAttackTargetRunThreshold && FVector::Dist(GetActorLocation(), AIC->AttackTarget->GetActorLocation()) < PunishingAttackTargetCloseThreshold;
	}
	return false;
}


void AEnemyCharacter::Interact_Implementation(AActor* InteractingActor)
{
	if (IsVulnerable() && InteractingActor->Implements<UInterface_Player>())
	{
		IInterface_Player::Execute_StabAction(InteractingActor, this);

		if (bIsBlocked)
		{
			OnBlockedExit.Broadcast();
		}

		GetCharacterMovement()->DisableMovement();
		DamagableComponent->bIsInvincible = true;
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation("Victim", GetActorLocation());
		GetMesh()->GetAnimInstance()->StopAllMontages(0.2f);
		GetMesh()->GetAnimInstance()->Montage_Play(StabVictimMontage);
	}
}

void AEnemyCharacter::EnterInteractionTargeting_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ENEMY INTERACTION TARGET"));
	if (IsVulnerable())
	{
		SetVulnerabilityWidgetEnabled(true);
	}
}

void AEnemyCharacter::ExitInteractionTargeting_Implementation()
{
	if (VulnerabilityFlag->IsVisible())
	{
		SetVulnerabilityWidgetEnabled(false);
	}
}

void AEnemyCharacter::ReturnToPlayerInputStarted(const FInputActionValue& Value)
{
	ReturnToPlayer(nullptr);
}

void AEnemyCharacter::Attack1Started()
{
	if(GetMesh()->GetAnimInstance()->Montage_IsPlaying(Attack1Montage) == false)
		GetMesh()->GetAnimInstance()->Montage_Play(Attack1Montage);
}

void AEnemyCharacter::OnGameLoadedEvent()
{
	AIC->OnGameLoadedStateSetup();
	GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);

	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		// Stop the behavior tree
		UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(AIController->BrainComponent);
		if (BTComponent)
		{
			BTComponent->RestartTree(EBTRestartMode::CompleteRestart);

			AIController->BrainComponent->Cleanup();

			UBehaviorTree* BehaviorTree = Tree; 
			if (BehaviorTree)
			{
				AIController->RunBehaviorTree(BehaviorTree);
			}
		}
	}

	if (AIC->AiState == EAIStates::Dead)
	{
		DamagableComponent->bIsInvincible = true;

		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);

		PerceptionComponent->PausePerceptionTimerLoop();
	}
	else
	{
		DamagableComponent->bIsInvincible = false;

		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
		GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		PerceptionComponent->StartPerceptionTimerLoop();
	}

	Super::OnGameLoadedEvent();

}

void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ReturnToPlayerAction, ETriggerEvent::Started, this, &AEnemyCharacter::ReturnToPlayerInputStarted);
		EnhancedInputComponent->BindAction(Attack_1, ETriggerEvent::Started, this, &AEnemyCharacter::Attack1Started);
	}
}

void AEnemyCharacter::SetupPossession()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, "SETUP POSSESSION");

	POVCamera->SetActive(false);
	FPCamera->SetActive(true);

	PossessedStartLocation = GetActorLocation();
	GetCapsuleComponent()->SetCapsuleRadius(PossessedCapsuleRadius);

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;
	CameraSpringArm->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = true;

	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::KeepRelativeTransform;
	AttachmentRules.LocationRule = EAttachmentRule::KeepWorld;
	AttachmentRules.RotationRule = EAttachmentRule::KeepRelative;
	AttachmentRules.ScaleRule = EAttachmentRule::KeepRelative;

	GetMesh()->AttachToComponent(MeshSpringArm, AttachmentRules);


	if (AController* MyController = GetController())
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(MyController))
		{
			if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
				{
					UE_LOG(LogTemp, Warning, TEXT("Adding mapping context in BeginPlay for %s"), *GetName());
					Subsystem->ClearAllMappings();
					Subsystem->AddMappingContext(DefaultMappingContext, 0);
				}
			}
		}
	}

	if (InputComponent)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "INPUT COMPONENT");
		SetupPlayerInputComponent(InputComponent);
	}

	
	GetGameMode()->GetPlayerCharacter()->DamagableComponent->OnDamage.AddDynamic(this, &AEnemyCharacter::ReturnToPlayer);

	EnableInput(Cast<APlayerController>(GetController()));

	SetDummyHead(true);
}

void AEnemyCharacter::SetupPOV()
{
	POVCamera->SetActive(true);
	FPCamera->SetActive(false);
	SetDummyHead(true);
}

void AEnemyCharacter::OnExitPOV()
{
	POVCamera->SetActive(false);
	FPCamera->SetActive(false);
	SetDummyHead(false);
}

void AEnemyCharacter::SetupLeaveBody()
{
	DisableInput(Cast<APlayerController>(GetController()));
	GetGameMode()->PlayerCharacter->DamagableComponent->OnDamage.RemoveDynamic(this, &AEnemyCharacter::ReturnToPlayer);

	FirstPersonPitch = 0.0f;
	GetCapsuleComponent()->SetCapsuleRadius(DefaultCapsuleRadius);

	FVector CapsuleRotation = GetCapsuleComponent()->GetComponentLocation();
	FVector TargetVector = CapsuleRotation + GetMesh()->GetRightVector();

	FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(CapsuleRotation, TargetVector);
	LookRotation.Roll = .0f;
	LookRotation.Pitch = .0f;
	//GetController()->SetControlRotation(LookRotation);

	FQuat ZeroQuat = FQuat::Identity;
	CameraSpringArm->SetRelativeRotation(ZeroQuat);
	MeshSpringArm->SetRelativeRotation(ZeroQuat);

	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::KeepRelativeTransform;
	AttachmentRules.LocationRule = EAttachmentRule::KeepWorld;
	AttachmentRules.RotationRule = EAttachmentRule::KeepRelative;
	AttachmentRules.ScaleRule = EAttachmentRule::KeepRelative;

	GetMesh()->AttachToComponent(GetCapsuleComponent(), AttachmentRules);
	POVCamera->SetActive(false);
	FPCamera->SetActive(false);
	SetDummyHead(false);

	SetActorRotation(LookRotation);
	

	UE_LOG(LogTemp, Error, TEXT("SetupLeaveBody - Set enemy possessed by AIC"));

	if (CurrentHitReaction == nullptr)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(LeftBodyMontage);
	}

	AIC->Possess(this);
	IInterface_AIAgent::Execute_ClearAttackFocus(this);

}

void AEnemyCharacter::ReturnToPlayer(AActor* DamageDealer)
{
	if (GetGameMode()->GetPlayerCharacter() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ReturnToPlayer - PlayerCharacter is null"));
	}

	GetWorldTimerManager().ClearTimer(PossessionBlendTimer);
	PossessionBlendTimer.Invalidate();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (bEnteredPOV)
	{
		if (POVTarget)
		{
			if (PlayerController)
			{
				bEnteredPOV = false;

				AEnemyCharacter* Enemy = IInterface_Enemy::Execute_GetOwnerCharacter(POVTarget);
				Enemy->OnPOVReceivedFinished.Broadcast();

				POVTarget = nullptr;
			}
		}
	}

	if (AimTarget)
	{
		IInterface_Enemy::Execute_SetIsTarget(AimTarget, false, false, false);
		AimTarget = nullptr;
	}

	if (PlayerController && GetGameMode()->GetPlayerCharacter())
	{
		APawn* PlayerPawn = Cast<APawn>(GetGameMode()->GetPlayerCharacter());
		PlayerController->Possess(PlayerPawn);
	}

	SetupLeaveBody();
}

void AEnemyCharacter::SetDummyHead(bool bIsFirstPerson)
{
	if (bIsFirstPerson)
	{
		GetMesh()->HideBoneByName("head", EPhysBodyOp::PBO_None);
	}
	else 
	{
		GetMesh()->UnHideBoneByName("head");
	}

	HeadDummyMesh->SetCastHiddenShadow(bIsFirstPerson);
}

void AEnemyCharacter::InitDummyHead()
{
	HeadDummyMesh->SetHiddenInGame(true, false);
	HeadDummyMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "dummy_head");
}

void AEnemyCharacter::UpdateWidgetRotation(UWidgetComponent* TargetWidget)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
		if (CameraManager)
		{
			FVector CameraLocation = CameraManager->GetCameraLocation();

			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(TargetWidget->GetComponentLocation(), CameraLocation);
			TargetWidget->SetWorldRotation(LookAtRotation);
		}
	}
}


float AEnemyCharacter::SetMovementSpeed_Implementation(EGait MovementSpeed)
{
	float TargetSpeed = 0.f;
	switch (MovementSpeed)
	{
	case EGait::None:
		TargetSpeed = 155.f;
		break;
	case EGait::Walk:
		TargetSpeed = 200.f;
		break;
	case EGait::Jog:
		TargetSpeed = 330.f;
		break;
	case EGait::Sprint:
		TargetSpeed = 430.f;
		break;
	default:
		break;
	}
	MovementState = MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = TargetSpeed;
	return TargetSpeed;
}

APatrolRoute* AEnemyCharacter::GetPatrolRoute_Implementation()
{
	if (AIC->bAlertMode && AlertPatrolRoute != nullptr)
	{
		return AlertPatrolRoute;
	}
	return PatrolRoute;
}

AAIAreaOfAction* AEnemyCharacter::GetAreaOfAction_Implementation()
{
	return AreaOfAction;
}

void AEnemyCharacter::Attack_Implementation()
{
	TPair<UAnimMontage*, FAIAttackSettings> SelectedAttack = TPair<UAnimMontage*, FAIAttackSettings>();
	
	for (auto& Elem : AttacksMap)
	{
		if (Elem.Value.TargetDistance == AIC->CurrentDistanceState)
		{
			SelectedAttack = Elem;
			break;
		}
	}
	
	bIsAttacking = true;
	if (SelectedAttack.Value.AnnounceMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(SelectedAttack.Value.AnnounceMontage);
	}

	AttackToPlay = SelectedAttack.Key;
	AIC->MoveToAttackTarget(AIC->AttackTarget, SelectedAttack.Value.AcceptanceRadius);
	/*
	if (CombatManager)
	{
		CombatManager->AddAttackingActor(this);
	}*/
}

void AEnemyCharacter::TryGrabAttack_Implementation()
{
	auto It = GrabAttackMap.CreateConstIterator(); //The Map is supposed to be used to check conditions for selection based on angle from target or more - right now there is only one grab attack
	UAnimMontage* FirstKey = It->Key;
	FAIGrabAttackSettings FirstValue = It->Value;

	bIsAttacking = true;

	AttackToPlay = FirstKey;
	AIC->MoveToAttackTarget(AIC->AttackTarget, FirstValue.AcceptanceRadius);

	UE_LOG(LogTemp, Warning, TEXT("EnemyCharacter - Try Grab Attack"));

	if (CombatManager)
	{
		CombatManager->AddAttackingActor(this);
	}
}

void AEnemyCharacter::DoGrabAttackDamage_Implementation(FDamageSettings DamageSettings)
{
	if (GrabVictim)
	{
		FHitResult hit = FHitResult();
		IInterface_Damagable::Execute_TakeDamage(GrabVictim, DamageSettings, hit, this);
	}
}

void AEnemyCharacter::StopGrabAttack_Implementation()
{
	GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);
}

void AEnemyCharacter::PunishAttackTraceTick_Implementation()
{
	float Radius = 40.f;
	FVector TraceStart = GetActorLocation();
	FVector TraceDirection = GetActorForwardVector() * 120.f;
	FVector TraceEnd = TraceStart + TraceDirection;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1);

	FHitResult HitResult = StaticFunctions::SphereCollisionTraceChannel(GetWorld(), TraceStart, TraceEnd, TraceTypeQuery, ActorsToIgnore, Radius);

	if (HitResult.bBlockingHit)
	{
		AActor* HitActor = HitResult.GetActor();

		if (HitActor == nullptr)
		{
			return;
		}

		bool bIsPlayer = IInterface_Perception::Execute_GetPerceivableComponent(HitActor)->GetTeam() == ETeam::Player;
		/*
		if (bIsPlayer == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s"), *HitActor->GetName());
			if (HitActor->GetAttachParentActor())
			{
				if (HitActor->GetAttachParentActor()->GetClass()->ImplementsInterface(UInterface_Damagable::StaticClass()))
				{
					bIsPlayer = true;
					HitActor = HitActor->GetAttachParentActor();
				}
			}
		}*/

		if (LastDamagedActors.Contains(HitActor))
			return;

		if (bIsPlayer )
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s"), *HitActor->GetName());
			LastDamagedActors.Add(HitActor);

			FDamageSettings GrabDamageSettings;
			GrabDamageSettings.bDamageInvincible = false;
			GrabDamageSettings.DamageAmount = 0;
			GrabDamageSettings.DamageResponse = EDamageResponse::None;
			GrabDamageSettings.DamageType = EDamageType::Physical;

			bool bWasDamaged = IInterface_Damagable::Execute_TakeDamage(HitActor, GrabDamageSettings, HitResult, this);
			if (bWasDamaged)
			{
				IInterface_Player::Execute_ReceiveGrabAttack(HitActor, this);
				GrabAttack(HitActor);
			}
		}
	}
}

void AEnemyCharacter::PunishAttackTraceEnd_Implementation()
{
	LastDamagedActors.Empty();
}

void AEnemyCharacter::PassiveToInvestigation_Implementation()
{
	GetMesh()->GetAnimInstance()->Montage_Play(InvestigateMontage);
}

void AEnemyCharacter::LookAround_Implementation()
{
	GetMesh()->GetAnimInstance()->Montage_Play(LookAroundMontage);
}

void AEnemyCharacter::ToggleEvasionFlag_Implementation(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		UpdateWidgetRotation(AttackFlagWidget);
		EvasionFlagTrace();
	}
	else
	{
		if (AttackFlagWidget->IsVisible() != bIsEnabled)
		{
			AttackFlagWidget->SetVisibility(false);
		}
		if (EvasionFlagTarget)
		{
			IInterface_Player::Execute_ToggleEvasionFlag(EvasionFlagTarget, false);
			EvasionFlagTarget = nullptr;
		}
	}
}

void AEnemyCharacter::AttackTraceStart_Implementation(FDamageSettings DamageSettings)
{
	CurrentDamageSettings = DamageSettings;
}

void AEnemyCharacter::AttackTraceTick_Implementation()
{
	float Radius = 40.f;
	FVector TraceStart = GetMesh()->GetSocketLocation(SpawnedWeapon->TraceStartSocketName);
	FVector TraceEnd = GetMesh()->GetSocketLocation(SpawnedWeapon->TraceEndSocketName);
	if (IsPlayerControlled())
	{
		Radius = 20.f;
		TraceStart = GetMesh()->GetSocketLocation(SpawnedWeapon->FPTraceStartSocketName);
		TraceEnd = GetMesh()->GetSocketLocation(SpawnedWeapon->FPTraceEndSocketName);
	}

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1);

	FHitResult HitResult = StaticFunctions::SphereCollisionTraceChannel(GetWorld(), TraceStart, TraceEnd, TraceTypeQuery, ActorsToIgnore, Radius);
	
	if (HitResult.bBlockingHit)
	{
		AActor* HitActor = HitResult.GetActor();

		if (HitActor == nullptr)
		{
			return;
		}

		bool bIsDamagable = HitActor->GetClass()->ImplementsInterface(UInterface_Damagable::StaticClass());
		
		if (bIsDamagable == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s"), *HitActor->GetName());
			if (HitActor->GetAttachParentActor())
			{
				if (HitActor->GetAttachParentActor()->GetClass()->ImplementsInterface(UInterface_Damagable::StaticClass()))
				{
					bIsDamagable = true;
					HitActor = HitActor->GetAttachParentActor();
				}
			}
		}

		if (LastDamagedActors.Contains(HitActor))
			return;

		if (bIsDamagable)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s"), *HitActor->GetName());
			LastDamagedActors.Add(HitActor);

			bool bWasDamaged = IInterface_Damagable::Execute_TakeDamage(HitActor, CurrentDamageSettings, HitResult, this);

			if (bWasDamaged && HitActor->Implements<UInterface_BaseCharacter>())
			{
				//UE_LOG(LogTemp, Warning, TEXT("HIT TRACE DEALT DAMAGE"));

				FTransform FxTransform;
				FxTransform.SetLocation(HitResult.ImpactPoint);
				FVector ImpactNormal = HitResult.ImpactNormal;
				FxTransform.SetRotation(SpawnedWeapon->FXTransformReference->GetComponentQuat());
				FxTransform.SetScale3D(FVector(1, 1, 1));
				ObjectPoolComponent->SpawnActorFromPool(FxTransform, ObjectPoolComponent->AttackHitPool);
				AttackHitFX(FxTransform);
			}

			if (IsPlayerControlled())
			{
				if (HitActor->GetClass()->ImplementsInterface(UInterface_Enemy::StaticClass()))
				{
					PerceivableComponent->Team = ETeam::AgainstAll;
				}
			}
			
			if (CurrentDamageSettings.DamageResponse == EDamageResponse::Knockback)
			{
				GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveAll(this);
				GetMesh()->GetAnimInstance()->StopAllMontages(1.f);
				//EndAttack();
			}
		}
	}
}

void AEnemyCharacter::AttackTraceEnd_Implementation()
{
	LastDamagedActors.Empty();
}

void AEnemyCharacter::SetIsTarget_Implementation(bool bIsTarget, bool bCheckIsPossessable, bool bUsingPerception)
{
	if (bIsTarget)
	{
		bool bActivatePossessionFlag = bCheckIsPossessable && CanBePossessed_Implementation();
		if (bActivatePossessionFlag)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, "ENEMY SET IS TARGET");
			PossessionFlag->SetVisibility(bActivatePossessionFlag);
		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "ENEMY SET IS - NOT - TARGET");
		PossessionFlag->SetVisibility(false);
	}
}

bool AEnemyCharacter::CanBePossessed_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, "CHECK CAN BE POSSESSED");

	return AIC->AiState == EAIStates::Passive;
}

AEnemyCharacter* AEnemyCharacter::GetOwnerCharacter_Implementation()
{
	return this;
}

void AEnemyCharacter::SetAttackFocus_Implementation()
{
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;

	if(AIC->LastAttackedTarget)
	AIC->SetFocus(AIC->LastAttackedTarget);
}

void AEnemyCharacter::ClearAttackFocus_Implementation()
{
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;
	AIC->ClearFocus(EAIFocusPriority::Move);
	AIC->ClearFocus(EAIFocusPriority::Default);
	AIC->ClearFocus(EAIFocusPriority::Gameplay);
}


void AEnemyCharacter::SeenNewActor_Implementation(AActor* SeenActor)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("%s ENEMY - SEEN NEW ACTOR"), *SeenActor->GetName()));
	//AIC->HandleSensedSight(SeenActor);
	AIC->OnTargetSeen.Broadcast(SeenActor);
}

UCameraComponent* AEnemyCharacter::GetActiveCamera() const
{
	return FPCamera;
}

void AEnemyCharacter::AttackHitFX_Implementation(FTransform HitTransform)
{
}

void AEnemyCharacter::PlaySelectedAttackMontage()
{
	GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveAll(this);

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	BlendingOutDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
		{
			EndAttack(Montage, false);
		});

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("PLAY MONTAGE"));
	GetMesh()->GetAnimInstance()->Montage_Play(AttackToPlay);
	GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendingOutDelegate, AttackToPlay);
}

void AEnemyCharacter::EndAttack(UAnimMontage* Montage, bool bForceInterrupt)
{
	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttackToPlay) && !bForceInterrupt)
		return;
	if (CombatManager)
	{
		CombatManager->RemoveAttackingActor(this);
	}


	/*if (AIC->OnAttackFinishedDelegate.IsBound())
	{
		UE_LOG(LogTemp, Error, TEXT("END ATTACK: OnAttackFinishedDelegate is bound."));
		AIC->OnAttackFinishedDelegate.Broadcast(AIC);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("END ATTACK: OnAttackFinishedDelegate is NOT bound."));
	}*/

	if (AIC->CurrentAttackTask)
	{
		//AIC->CurrentAttackTask->FinishLatentTask(*AIC->CurrentAttackTask->CachedOwnerComponent, EBTNodeResult::Succeeded);
		//AIC->CurrentAttackTask = nullptr;
		//UE_LOG(LogTemp, Error, TEXT("-- Attack task END"));
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("-- Attack task null"));

	}

	bIsAttacking = false;
	//AIC->OnAttackFinishedDelegate.Broadcast(AIC);
}

void AEnemyCharacter::SetSightSettings(EAIStates AiState)
{
	switch (AiState)
	{
	case EAIStates::Passive:
		PerceptionComponent->CurrentSightSettings = PassiveSightSettings;
		break;
	case EAIStates::Combat:
		PerceptionComponent->CurrentSightSettings = CombatSightSettings;
		break;
	case EAIStates::Investigating:
		PerceptionComponent->CurrentSightSettings = InvestigationSightSettings;
		break;
	case EAIStates::Seeking:
		PerceptionComponent->CurrentSightSettings = SeekingSightSettings;
		break;
	default:
		PerceptionComponent->CurrentSightSettings = PassiveSightSettings;
		break;
	}
}

void AEnemyCharacter::OnCombatEnter(FVector ActorLocation)
{
	GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, InvestigateMontage);
	GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, LookAroundMontage);
	GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, LeftBodyMontage);
	GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, CurrentWayPointMontage);
}

void AEnemyCharacter::EvasionFlagTrace()
{
	FVector TraceStart = GetActorLocation();
	FVector ActorForward = GetActorForwardVector() * EvasionTraceLenght;

	FVector TraceEnd = TraceStart + ActorForward;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	//ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	FHitResult HitResult = StaticFunctions::SphereCollisionTraceObject(GetWorld(), TraceStart, TraceEnd, ObjectTypes, ActorsToIgnore, 50.f);

	if (AActor* HitActor = HitResult.GetActor())
	{
		if (HitActor->GetClass()->ImplementsInterface(UInterface_Player::StaticClass()))
		{
			EvasionFlagTarget = HitActor;
			if (IInterface_Player::Execute_IsEvasionEnabled(HitActor))
			{
				IInterface_Player::Execute_ToggleEvasionFlag(HitActor, true);
				AttackFlagWidget->SetVisibility(true);
			}
		}
	}
	else if(EvasionFlagTarget)
	{
		IInterface_Player::Execute_ToggleEvasionFlag(EvasionFlagTarget, false);
		AttackFlagWidget->SetVisibility(false);
		EvasionFlagTarget = nullptr;
	}
}

bool AEnemyCharacter::IsVulnerable()
{
	if (AIC->AiState == EAIStates::Passive || bIsBlocked)
	{
		return true;
	}

	return false;
}

void AEnemyCharacter::SetVulnerabilityWidgetEnabled(bool bEnabled)
{
	if (bEnabled == false && bIsBlocked == false)
	{
		VulnerabilityFlag->SetVisibility(false);
	}
	else if (bEnabled)
	{
		VulnerabilityFlag->SetVisibility(true);
	}
}

void AEnemyCharacter::GrabAttack(AActor* Victim)
{
	//DamagableComponent->bIsInvincible = true;
	GrabVictim = Victim;

	AttackToPlay = GrabAttackMap.Find(AttackToPlay)->GrabPairedAttack;
	
	FVector WarpLocation = StaticFunctions::GetLocationAtDistanceXFromActorB(this, Victim, 100.f);
	FRotator WarpRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Victim->GetActorLocation());
	//GetController()->SetControlRotation(WarpRotation);

	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation("grab", WarpLocation, WarpRotation);

	GetMesh()->GetAnimInstance()->OnMontageBlendingOut.RemoveAll(this);

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	BlendingOutDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
		{
			//DamagableComponent->bIsInvincible = false;
			GrabVictim = nullptr; 
			FRotator rot = GetActorRotation();
			rot.Pitch = 0;
			rot.Roll = 0;
			SetActorRotation(rot);
			EndAttack(Montage, false);
		});

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("PLAY MONTAGE"));
	GetMesh()->GetAnimInstance()->Montage_Play(AttackToPlay);
	GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendingOutDelegate, AttackToPlay);
}

void AEnemyCharacter::GrabAttackInterruption()
{
	if (GrabVictim)
	{
		IInterface_Player::Execute_GrabAttackInterrupted(GrabVictim);
	}

	GrabVictim = nullptr;
	FRotator rot = GetActorRotation();
	rot.Pitch = 0;
	rot.Roll = 0;
	SetActorRotation(rot);
}

