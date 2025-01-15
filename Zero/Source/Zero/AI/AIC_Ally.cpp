// Fill out your copyright notice in the Description page of Project Settings.


#include "AIC_Ally.h"
#include "ZeroGameMode.h"
#include "Interface_GameMode.h"
#include "PlayerAllyCharacter.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

void AAIC_Ally::BeginPlay()
{
	Super::BeginPlay();

	
}

void AAIC_Ally::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (APlayerAllyCharacter* const character = Cast<APlayerAllyCharacter>(InPawn))
	{
		CharacterRef = character;
		CharacterRef->AIC = this;
		if (UBehaviorTree* tree = character->GetBehviorTree())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Black, ("INIT BEHAVIOR TREE"));
			UBlackboardComponent* b;
			UseBlackboard(tree->BlackboardAsset, b);
			Blackboard = b;
			RunBehaviorTree(tree);
		}
	}
}

void AAIC_Ally::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDistanceState();

	if (AiState == EAIStates::Combat)
	{
		GetBlackboardComponent()->SetValueAsBool("IsAttacking", CharacterRef->bIsAttacking);
	}
}

void AAIC_Ally::UpdateDistanceState()
{
	CurrentDistanceState = GetDistanceState();

	GetBlackboardComponent()->SetValueAsEnum("DistanceState", static_cast<uint8>(CurrentDistanceState));

}

EDistanceState AAIC_Ally::GetDistanceState()
{
	AActor* ActorToGetDistance = nullptr;

	if (AiState == EAIStates::Passive)
	{
		ActorToGetDistance = PlayerRef;
	}
	else if (AiState == EAIStates::Combat)
	{
		ActorToGetDistance = AttackTarget;
	}

	if(ActorToGetDistance == nullptr)
		return EDistanceState::Far;

	DistanceFromTarget = FVector::Distance(ActorToGetDistance->GetActorLocation(), CharacterRef->GetActorLocation());

	if (DistanceFromTarget > 600.f)
	{
		return EDistanceState::Far;
	}
	else if (DistanceFromTarget > 250.f)
	{
		return EDistanceState::Mid;
	}

	return EDistanceState::Close;
}

void AAIC_Ally::OnAttackMoveCompleted(FAIRequestID, const FPathFollowingResult& Result)
{

	if (Result.IsInterrupted())
	{
		UE_LOG(LogTemp, Warning, TEXT("MOVEMENT ATTACK: MOVEMENT INTERRUPTED"));

		if (BrainComponent->IsActive() == false)
		{
			return;
		}

		// Log the reason for the interruption
		switch (Result.Code)
		{
		case EPathFollowingResult::Aborted:
			//CharacterRef->PlaySelectedAttackMontage();
			CharacterRef->EndAttack(nullptr, false);
			UE_LOG(LogTemp, Warning, TEXT("Reason: Path following aborted."));
			break;
		case EPathFollowingResult::Blocked:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Path was blocked."));
			break;
		case EPathFollowingResult::OffPath:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Character went off the path."));
			break;
		case EPathFollowingResult::Invalid:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Invalid."));
		case EPathFollowingResult::Success:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Success."));

		default:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Unknown interruption."));
			break;
		}
	}
	else if (Result.IsFailure())
	{
		UE_LOG(LogTemp, Warning, TEXT("MOVEMENT ATTACK: FAILED"));

		if (BrainComponent->IsActive() == false)
		{
			return;
		}

		switch (Result.Code)
		{
		case EPathFollowingResult::Aborted:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Path following aborted."));
			CharacterRef->EndAttack(nullptr, false);
			break;
		case EPathFollowingResult::Blocked:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Path was blocked."));
			break;
		case EPathFollowingResult::OffPath:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Character went off the path."));
			break;
		case EPathFollowingResult::Invalid:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Invalid."));
		case EPathFollowingResult::Success:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Success."));
			CharacterRef->PlaySelectedAttackMontage();
		default:
			UE_LOG(LogTemp, Warning, TEXT("Reason: Unknown interruption."));
			CharacterRef->EndAttack(nullptr, false);
			break;
		}
	}
	else if (Result.IsSuccess())
	{
		UE_LOG(LogTemp, Warning, TEXT("MOVEMENT ATTACK: SUCCESS"));

		if (BrainComponent->IsActive() == false)
		{
			return;
		}

		CharacterRef->PlaySelectedAttackMontage();
	}

	//CharacterRef->EndAttack(nullptr);
	GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
}

void AAIC_Ally::TryExecuteMoveToTask(FVector Location)
{
	if (AiState == EAIStates::Passive)
	{
		UE_LOG(LogTemp, Warning, TEXT("ALLY: TRY MOVE TO TASK"));
		GetBlackboardComponent()->SetValueAsBool("HasTask", true);
		GetBlackboardComponent()->SetValueAsVector("TaskLocation", Location);
	}
}

void AAIC_Ally::SetAttackTarget(AActor* Actor)
{
	if (AiState == EAIStates::Dead)
		return;

	if (AttackTarget != Actor)
	{
		if (AttackTarget != nullptr)
		{
			UComponent_Damagable* OldTargetDamagable = IInterface_Damagable::Execute_GetDamagableComponent(AttackTarget);
			if (OldTargetDamagable)
			{
				OldTargetDamagable->OnDeath.RemoveDynamic(this, &AAIC_Ally::HandleAttackTargetDeath);
			}
		}

		if (Actor != nullptr)
		{
			UComponent_Damagable* NewTargetDamagable = IInterface_Damagable::Execute_GetDamagableComponent(Actor);
			if (NewTargetDamagable)
			{
				if (!NewTargetDamagable->OnDeath.IsAlreadyBound(this, &AAIC_Ally::HandleAttackTargetDeath))
				{
					NewTargetDamagable->OnDeath.AddDynamic(this, &AAIC_Ally::HandleAttackTargetDeath);
				}
			}
		}
	}

	AttackTarget = Actor;

	AiState = EAIStates::Combat;
	GetBlackboardComponent()->SetValueAsObject("AttackTarget", AttackTarget);
	GetBlackboardComponent()->SetValueAsEnum("AiStates", static_cast<uint8>(AiState));

	
}

void AAIC_Ally::EndMoveToTask()
{
	GetBlackboardComponent()->SetValueAsBool("HasTask", false);
}

void AAIC_Ally::HandleAttackTargetDeath(AActor* Actor)
{
	AttackTarget = nullptr;
	GetBlackboardComponent()->SetValueAsObject("AttackTarget", nullptr);

	GetBlackboardComponent()->SetValueAsBool("HasTask", false);

	if (AiState == EAIStates::Dead)
		return;

	AiState = EAIStates::Passive;
	GetBlackboardComponent()->SetValueAsEnum("AiStates", static_cast<uint8>(AiState));
}

void AAIC_Ally::OnDeath()
{
	AiState = EAIStates::Dead;
	GetBlackboardComponent()->SetValueAsEnum("AiStates", static_cast<uint8>(AiState));
	StopMovement();
}

void AAIC_Ally::SetStateAsPassive_Implementation()
{
	AiState = EAIStates::Passive;

	GetBlackboardComponent()->SetValueAsEnum("AiStates", static_cast<uint8>(AiState));
}

void AAIC_Ally::HandlePlayerDamaged(AActor* Dealer)
{
	if (Dealer && AttackTarget == nullptr)
	{
		SetAttackTarget(Dealer);
	}
}

void AAIC_Ally::SetPlayerRef()
{
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(GetWorld());
	AZeroGameMode* GameMode = IInterface_GameMode::Execute_GetZeroGameMode(GameModeBase);
	if (GameMode)
	{
		PlayerRef = Cast<AActor>(GameMode->GetPlayerCharacter());

		if (GetBlackboardComponent())
		{
			GetBlackboardComponent()->SetValueAsObject("PlayerCharacter", PlayerRef);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AIC_Ally BeginPlay: BlackboardComponent is null, cannot set PlayerCharacter."));
		}

		if (PlayerRef)
		{
			IInterface_Damagable::Execute_GetDamagableComponent(PlayerRef)->OnDamage.AddDynamic(this, &AAIC_Ally::HandlePlayerDamaged);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AIC_Ally BeginPlay: ZeroGameMode is null, cannot get player ref."));
	}
}

void AAIC_Ally::RemovePlayerRef()
{
	if (PlayerRef)
	{
		PlayerRef = nullptr;
		IInterface_Damagable::Execute_GetDamagableComponent(PlayerRef)->OnDamage.RemoveDynamic(this, &AAIC_Ally::HandlePlayerDamaged);
	}
}
