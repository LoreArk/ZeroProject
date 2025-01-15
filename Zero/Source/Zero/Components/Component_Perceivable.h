
#pragma once

#include "CoreMinimal.h"
#include "PerceptionSystemStructs.h"
#include "Components/ActorComponent.h"
#include "Component_Perceivable.generated.h"

class ABaseCharacter;

UENUM(BlueprintType)
enum class ETeam : uint8
{
	None		UMETA(DisplayName = "None"),
	Player		UMETA(DisplayName = "Player"),
	Enemies     UMETA(DisplayName = "Enemies"),
	PlayerAlly	UMETA(DisplayName = "PlayerAlly"),
	AgainstAll	UMETA(DisplayName = "AgainstAll"),
	PlayerDummy UMETA(DisplayName = "PlayerDummy")
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMakeNoise, AActor*, NoiseMaker);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZERO_API UComponent_Perceivable : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UComponent_Perceivable();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnMakeNoise OnMakeNoise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETeam Team;

	UFUNCTION()
	bool IsSameTeam(ETeam ActorTeam);

	UFUNCTION()
	void MakeNoise(FNoiseStimulus Noise);

	UFUNCTION()
	ETeam GetTeam();

	UFUNCTION()
	FVector GetSightTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perceivable Settings")
	FNoiseStimulus NoiseSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perceivable Settings")
	FName SightTargetSocket;

	UPROPERTY()
	ABaseCharacter* OwnerCharacter;
};
