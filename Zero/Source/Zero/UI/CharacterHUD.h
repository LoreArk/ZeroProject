// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Component_Damagable.h"
#include "CharacterHUD.generated.h"

/**
 * 
 */

UCLASS()
class ZERO_API UCharacterHUD : public UUserWidget
{
	GENERATED_BODY()

public:

	TWeakObjectPtr<UComponent_Damagable> DamagableComponent;

	virtual void SetDamagableComponent(UComponent_Damagable* DamagableComp);

	UFUNCTION()
	void InitHUD(APossessableCharacter* OwnerChar);

	UFUNCTION()
	void UpdateHealthBar();

	UFUNCTION()
	void UpdateManaBar();

	UFUNCTION()
	void ToggleCrosshair(bool bEnabled);

	UFUNCTION(BlueprintNativeEvent)
	void PlayDamageEffect(AActor* Dealer);
	void PlayDamageEffect_Implementation(AActor* Dealer);

	UFUNCTION()
	void SetVisible();

	UFUNCTION()
	void SetHidden();

protected:


	UPROPERTY(meta = (BindWidget))
	UImage* HealthImage;

	UPROPERTY(meta = (BindWidget))
	UImage* EmptyHealthImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* HealthBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* EmptyHealthBox;

	UPROPERTY(meta = (BindWidget))
	UImage* ManaImage;

	UPROPERTY(meta = (BindWidget))
	UImage* EmptyManaImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* EmptyManaBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* ManaBox;


	TArray<UImage*> CreatedHealthUnits;
	TArray<UImage*> CreatedEmptyHealthUnits;

	TArray<UImage*> CreatedManaUnits;
	TArray<UImage*> CreatedEmptyManaUnits;

	UPROPERTY(meta = (BindWidget))
	UImage* Crosshair;
	//void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

};
