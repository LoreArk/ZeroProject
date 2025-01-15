// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterHUD.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API UPlayerHUD : public UCharacterHUD
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* EvadeIcon;

	UFUNCTION(BlueprintNativeEvent)
	void SetEvadeIconEnabled(bool bEnabled);
	void SetEvadeIconEnabled_Implementation(bool bEnabled);
};
