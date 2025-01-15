// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "KeyItem.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct ZERO_API FKeySettings
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Settings")
    FName Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Settings")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Settings")
    FName ID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Settings")
    UTexture2D* Icon;


    FKeySettings()
        : Name("Name"), Description(), ID(), Icon()
    {}
};


UCLASS()
class ZERO_API AKeyItem : public AItem
{
	GENERATED_BODY()
	
public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FKeySettings Settings;


    virtual void Interact_Implementation(AActor* InteractingActor) override;
};
