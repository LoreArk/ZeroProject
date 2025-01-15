// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAllyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "DamageStructs.h"
#include "PlayerCharacter.h"
#include "StaticFunctions.h"
#include "BehaviorTree/BehaviorTree.h"
#include "ZeroGameMode.h"


APlayerAllyCharacter::APlayerAllyCharacter()
{
	ObjectPoolComponent = CreateDefaultSubobject<UComponent_ObjectPool>(TEXT("Object Pool Component"));

}

void APlayerAllyCharacter::BeginPlay()
{
	Super::BeginPlay();

	AIC = Cast<AAIC_Ally>(GetController());

	if (AIC == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "AIC not found!");
	}

	DamagableComponent->OnDamage.AddDynamic(this, &APlayerAllyCharacter::OnDamageReceived);
}

void APlayerAllyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerAllyCharacter::Attack_Implementation()
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
}

float APlayerAllyCharacter::SetMovementSpeed_Implementation(EGait MovementSpeed)
{
	float TargetSpeed = 0.f;
	switch (MovementSpeed)
	{
	case EGait::None:
		TargetSpeed = 100.f;
		break;
	case EGait::Walk:
		TargetSpeed = 200.f;
		break;
	case EGait::Jog:
		TargetSpeed = 330.f;
		break;
	case EGait::Sprint:
		TargetSpeed = 700.f;
		break;
	default:
		break;
	}
	MovementState = MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = TargetSpeed;
	return TargetSpeed;
}

void APlayerAllyCharacter::SetAttackFocus_Implementation()
{
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	if (AIC->AttackTarget)
		AIC->SetFocus(AIC->AttackTarget);
}

void APlayerAllyCharacter::ClearAttackFocus_Implementation()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	AIC->ClearFocus(EAIFocusPriority::Move);
	AIC->ClearFocus(EAIFocusPriority::Default);
	AIC->ClearFocus(EAIFocusPriority::Gameplay);
}

void APlayerAllyCharacter::SetMoveToTaskLocation_Implementation(FVector Location)
{
	if (AIC == nullptr)
	{

		UE_LOG(LogTemp, Warning, TEXT("ALLY: AIC IS NULL"));
	}
	AIC->TryExecuteMoveToTask(Location);
}

void APlayerAllyCharacter::SetAttackTarget_Implementation(AActor* Target)
{
	AIC->SetAttackTarget(Target);
}

void APlayerAllyCharacter::StopMoveToTask_Implementation()
{
	AIC->EndMoveToTask();
}

void APlayerAllyCharacter::OnDeathEvent(AActor* DeadActor)
{
	IInterface_AIAgent::Execute_ClearAttackFocus(this);

	GetMesh()->GetAnimInstance()->Montage_Play(DamagableComponent->DeathMontage);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);

	AIC->OnDeath();
}

void APlayerAllyCharacter::PlayHitReaction_Implementation(UAnimMontage* Montage)
{
	EndAttack(Montage, true);

	Super::PlayHitReaction_Implementation(Montage);
}

void APlayerAllyCharacter::OnDamageReceived(AActor* Dealer)
{
	if (AIC)
	{
		if (AIC->AttackTarget == nullptr && Dealer != nullptr)
		{
			AIC->SetAttackTarget(Dealer);
		}
	}
}

UBehaviorTree* APlayerAllyCharacter::GetBehviorTree()
{
	return Tree;
}

void APlayerAllyCharacter::PlaySelectedAttackMontage()
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

void APlayerAllyCharacter::EndAttack(UAnimMontage* Montage, bool bForceInterrupt)
{
	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttackToPlay) && !bForceInterrupt)
		return;

	bIsAttacking = false;


	AIC->OnAttackFinishedDelegate.Broadcast(AIC);
	UE_LOG(LogTemp, Warning, TEXT("END ATTACK"));
}

void APlayerAllyCharacter::AttackTraceStart_Implementation(FDamageSettings DamageSettings)
{
	CurrentDamageSettings = DamageSettings;

}

void APlayerAllyCharacter::AttackTraceTick_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("END ATTACK"));

	float Radius = 40.f;
	FVector TraceStart = GetMesh()->GetSocketLocation(TraceStartName);
	FVector TraceEnd = GetMesh()->GetSocketLocation(TraceEndName);
	
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

			if (bWasDamaged)
			{
				//UE_LOG(LogTemp, Warning, TEXT("HIT TRACE DEALT DAMAGE"));

				FTransform FxTransform;
				FVector ImpactNormal = HitResult.ImpactNormal;
				FxTransform.SetLocation(HitResult.ImpactPoint);
				FxTransform.SetRotation(ImpactNormal.Rotation().Quaternion());
				ObjectPoolComponent->SpawnActorFromPool(FxTransform, ObjectPoolComponent->AttackHitPool);
				//AttackHitFX(FxTransform);
				PerceivableComponent->Team = ETeam::PlayerAlly;
			}
		}
	}
}

void APlayerAllyCharacter::AttackTraceEnd_Implementation()
{
	LastDamagedActors.Empty();
}
