// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyGunner.h"
#include "StaticFunctions.h"
#include "Kismet/KismetMathLibrary.h"


void AEnemyGunner::BeginPlay()
{
	Super::BeginPlay();

	OnTargetAimed.AddDynamic(this, &AEnemyGunner::StartShootTimer);
	EvasionTraceLenght = 10000.f;
}

void AEnemyGunner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AIC)
	{
		if (AIC->AiState == EAIStates::Combat || AIC->AiState == EAIStates::Seeking)
		{
			bAttackAim = true;
		}
		else
			bAttackAim = false;
	}
}

void AEnemyGunner::Aim(const FInputActionValue& Value)
{
	Super::Aim(Value);

	if (StatsHud.IsValid())
	{
		StatsHud->ToggleCrosshair(true);
	}
}

void AEnemyGunner::AimCompleted(const FInputActionValue& Value)
{
	Super::AimCompleted(Value);

	if (StatsHud.IsValid())
	{
		StatsHud->ToggleCrosshair(false);
	}
}

void AEnemyGunner::Attack_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ATTACK IMPLEMENTATION"));

	AttackTimer = 0;

	bIsAttacking = true;
	IInterface_AIAgent::Execute_SetAttackFocus(this);
	GetWorldTimerManager().SetTimer(AimTimerHandle, this, &AEnemyGunner::AimTraceForAttackTarget, 0.1f, true);
}

void AEnemyGunner::AimTraceForAttackTarget()
{
	AttackTimer += 0.1f;

	if (AttackTimer > 2.f)
	{
		GetWorldTimerManager().PauseTimer(AimTimerHandle);
		EndAttack(nullptr, false);
	}

	FVector TraceStart = GetActorLocation();
	//FVector AimForward = GetActorForwardVector() * 20000.f;
	FRotator AimRotation = GetBaseAimRotation();
	FVector AimForward = AimRotation.Vector() * 20000.f;

	FVector TraceEnd = TraceStart + AimForward;


	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	FHitResult HitResult = StaticFunctions::SphereCollisionTraceObject(GetWorld(), TraceStart, TraceEnd, ObjectTypes, ActorsToIgnore, 1.f);

	if (AActor* HitActor = HitResult.GetActor())
	{
		if (HitActor == AIC->AttackTarget)
		{
			UE_LOG(LogTemp, Warning, TEXT("AIM"));
			TargetShootLocation = HitActor->GetActorLocation();
			OnTargetAimed.Broadcast();
		}
	}
}


void AEnemyGunner::AttackTraceTick_Implementation()
{
	SpawnedWeapon->PlayFX();

	FVector ShootLocationDirection;
	FVector TraceStart;

	if (IsPlayerControlled() == false)
	{
		ShootLocationDirection = (TargetShootLocation - GetActorLocation()) * 20000.f;
		TraceStart = GetActorLocation();
	}
	else
	{
		ShootLocationDirection = GetActiveCamera()->GetForwardVector() * 20000.f;
		TraceStart = GetActiveCamera()->GetComponentLocation();
	}
	

	FVector TraceEnd = TraceStart + ShootLocationDirection;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_EngineTraceChannel1);

	FHitResult HitResult = StaticFunctions::SphereCollisionTraceChannel(GetWorld(), TraceStart, TraceEnd, TraceTypeQuery, ActorsToIgnore, 0.1f);

	if (HitResult.bBlockingHit && HitResult.GetActor())
	{
		AActor* HitActor = HitResult.GetActor();

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

			bool bTargetDamaged = IInterface_Damagable::Execute_TakeDamage(HitActor, CurrentDamageSettings, HitResult, this);

			if (bTargetDamaged)
			{
				FTransform FxTransform;
				FxTransform.SetLocation(HitResult.ImpactPoint);
				FVector ImpactNormal = HitResult.ImpactNormal;;
				FxTransform.SetRotation(HitResult.ImpactNormal.ToOrientationQuat());
				FxTransform.SetScale3D(FVector(1, 1, 1));
				ObjectPoolComponent->SpawnActorFromPool(FxTransform, ObjectPoolComponent->AttackHitPool);

			}

			if (IsPlayerControlled())
			{
				if (HitActor->GetClass()->ImplementsInterface(UInterface_Enemy::StaticClass()))
				{
					PerceivableComponent->Team = ETeam::AgainstAll;
				}
			}
		}
	}
}

void AEnemyGunner::Attack1Started()
{
	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(Attack1Montage) == false && Shots > 0)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Attack1Montage);
		Shots--;
		OnFPShoot.Broadcast(Shots);
	}
}

void AEnemyGunner::Shoot()
{
	IInterface_AIAgent::Execute_ClearAttackFocus(this);

	GetMesh()->GetAnimInstance()->Montage_Play(ShootMontage);

	UE_LOG(LogTemp, Warning, TEXT("SHOOT"));

	bIsAttacking = false;
}

void AEnemyGunner::StartShootTimer()
{
	GetWorldTimerManager().PauseTimer(AimTimerHandle);
	
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemyGunner::Shoot, AimToShootDelay, false);
}
