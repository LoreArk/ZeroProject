// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Component_Damagable.h"
#include "Component_Perception.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MotionWarpingComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	PerceivableComponent = CreateDefaultSubobject<UComponent_Perceivable>(TEXT("Perceivable Component"));

	DamagableComponent = CreateDefaultSubobject<UComponent_Damagable>(TEXT("Damagable Component"));
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warping Component"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	DamagableComponent->OnDeath.AddDynamic(this, &ABaseCharacter::OnDeathEvent);
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShowDebugs)
	{
		DebugControlRotationArrow();
	}

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ABaseCharacter::TakeDamage_Implementation(FDamageSettings DamageSettings, FHitResult HitResult, AActor* DamageDealer)
{
	if (DamagableComponent->HitReactions.Num() == 0)
		UE_LOG(LogTemp, Error, TEXT("UComponenet_Damagable::DamageResponse - HitReaction map is empty!"));
	
	UE_LOG(LogTemp, Error, TEXT("TAKE DAMAGE"));

	bool bWasDamaged = DamagableComponent->TakeDamage(DamageSettings, DamageDealer);

	return bWasDamaged;
}

void ABaseCharacter::PlayHitReaction_Implementation(UAnimMontage* Montage)
{
	CurrentHitReaction = Montage;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &ABaseCharacter::EndHitReaction);
	}

	if (AnimInstance)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Montage);
		GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ABaseCharacter::EndHitReaction);
	}
}

UComponent_Damagable* ABaseCharacter::GetDamagableComponent_Implementation()
{
	return DamagableComponent;
}

UComponent_Perceivable* ABaseCharacter::GetPerceivableComponent_Implementation()
{
	return PerceivableComponent;
}

FVector ABaseCharacter::GetSightTarget_Implementation()
{
	return GetMesh()->GetSocketLocation(PerceivableComponent->SightTargetSocket);
}

bool ABaseCharacter::GetIsCrouching_Implementation()
{
	return GetCharacterMovement()->IsCrouching();
}

void ABaseCharacter::EndHitReaction(UAnimMontage* Montage, bool bInterrupted)
{
	CurrentHitReaction = nullptr;
}

void ABaseCharacter::OnDeathEvent(AActor* DeadActor)
{
}

void ABaseCharacter::DebugControlRotationArrow()
{
	if (Controller)
	{
		// Get the current control rotation
		FRotator ControlRotation = Controller->GetControlRotation();

		// Define the start location (character location)
		FVector StartLocation = GetActorLocation();

		// Define the direction based on control rotation (arrow length is 200 units)
		FVector EndLocation = StartLocation + (FRotationMatrix(ControlRotation).GetUnitAxis(EAxis::X) * 200.0f);

		// Define arrow color (Green in this case)
		FColor ArrowColor = FColor::Green;

		// Draw the debug arrow
		DrawDebugDirectionalArrow(GetWorld(), StartLocation, EndLocation, 100.0f, ArrowColor, false, -1.0f, 0, 5.0f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Controller found!"));
	}
}

