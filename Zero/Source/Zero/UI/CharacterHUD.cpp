// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterHUD.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Component_Damagable.h"
#include "PossessableCharacter.h"
#include "UObject/ConstructorHelpers.h"

void UCharacterHUD::SetDamagableComponent(UComponent_Damagable* DamagableComp)
{
	DamagableComponent = DamagableComp;

	if (DamagableComponent.IsValid())
	{
		DamagableComponent->OnDamage.AddDynamic(this, &UCharacterHUD::PlayDamageEffect);
	}
}

void UCharacterHUD::InitHUD(APossessableCharacter* OwnerChar)
{
	OwnerChar->OnPOVExit.AddDynamic(this, &UCharacterHUD::SetVisible);
	OwnerChar->OnPOVStart.AddDynamic(this, &UCharacterHUD::SetHidden);
}

void UCharacterHUD::UpdateHealthBar()
{
	if (DamagableComponent.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterHUD - UpdateHealthBar - missing reference to DamagableComponent"));
		return;
	}
	if (!HealthImage || !EmptyHealthImage || !HealthBox || !EmptyHealthBox)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterHUD - UpdateHealthBar - One or more HUD elements are null!"));
		return;
	}
	if (HealthImage && EmptyHealthImage && HealthBox && EmptyHealthBox)
	{
		if (CreatedHealthUnits.Num() < DamagableComponent->MaxHealth - 1)
		{
			for (int32 i = CreatedHealthUnits.Num(); i < DamagableComponent->MaxHealth - 1; ++i)
			{
				UImage* NewImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), FName(*FString::Printf(TEXT("HealthImage_%d"), i)));
				NewImage->SetBrush(HealthImage->Brush);
				CreatedHealthUnits.Add(NewImage);
				UHorizontalBoxSlot* NewHealthSlot = HealthBox->AddChildToHorizontalBox(NewImage);
				NewHealthSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);

				NewHealthSlot->SetPadding(FMargin(0.0f, 0.0f, 5.0f, 0.0f));

				UImage* NewEmptyImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), FName(*FString::Printf(TEXT("EmptyHealthImage_%d"), i)));
				NewEmptyImage->SetBrush(EmptyHealthImage->Brush);
				CreatedEmptyHealthUnits.Add(NewEmptyImage);
				UHorizontalBoxSlot* NewEmptyHealthSlot = EmptyHealthBox->AddChildToHorizontalBox(NewEmptyImage);
				NewEmptyHealthSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);

				NewEmptyHealthSlot->SetPadding(FMargin(0.0f, 0.0f, 5.0f, 0.0f));


				UE_LOG(LogTemp, Warning, TEXT("Health: %d, MaxHealth: %d, CreatedHealthUnits: %d"),
					DamagableComponent->Health, DamagableComponent->MaxHealth, CreatedHealthUnits.Num());

			}
		}

		for (UImage* Unit : CreatedHealthUnits)
		{
			Unit->SetVisibility(ESlateVisibility::Hidden);
		}

		if (DamagableComponent->Health > 0)
		{
			HealthImage->SetVisibility(ESlateVisibility::Visible);
			for (int32 i = 0; i < DamagableComponent->Health - 1; ++i)
			{
				if (i < CreatedHealthUnits.Num())
				{
					CreatedHealthUnits[i]->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
		else
		{
			HealthImage->SetVisibility(ESlateVisibility::Hidden);
		}

		for (UImage* EmptyHealth : CreatedEmptyHealthUnits)
		{
			EmptyHealth->SetVisibility(ESlateVisibility::Hidden);
		}

		if (DamagableComponent->MaxHealth > 0)
		{
			EmptyHealthImage->SetVisibility(ESlateVisibility::Visible);
			for (int32 i = 0; i < DamagableComponent->MaxHealth - 1; ++i)
			{
				if (i < CreatedEmptyHealthUnits.Num())
				{
					CreatedEmptyHealthUnits[i]->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
		else
		{
			EmptyHealthImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UCharacterHUD::UpdateManaBar()
{
	if (ManaImage && EmptyManaImage && ManaBox && EmptyManaBox)
	{
		if (CreatedManaUnits.Num() < DamagableComponent->MaxMana - 1)
		{
			for (int32 i = CreatedManaUnits.Num(); i < DamagableComponent->MaxMana - 1; ++i)
			{
				UImage* NewImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), FName(*FString::Printf(TEXT("ManaImage_%d"), i)));
				NewImage->SetBrush(ManaImage->Brush);
				CreatedManaUnits.Add(NewImage);
				UHorizontalBoxSlot* NewHealthSlot = ManaBox->AddChildToHorizontalBox(NewImage);
				NewHealthSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);

				NewHealthSlot->SetPadding(FMargin(0.0f, 0.0f, 5.0f, 0.0f));

				UImage* NewEmptyImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), FName(*FString::Printf(TEXT("EmptyManaImage_%d"), i)));
				NewEmptyImage->SetBrush(EmptyManaImage->Brush);
				CreatedEmptyManaUnits.Add(NewEmptyImage);
				UHorizontalBoxSlot* NewEmptyHealthSlot = EmptyManaBox->AddChildToHorizontalBox(NewEmptyImage);
				NewEmptyHealthSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);

				NewEmptyHealthSlot->SetPadding(FMargin(0.0f, 0.0f, 5.0f, 0.0f));
			}
		}
		
		for (UImage* Unit : CreatedManaUnits)
		{
			Unit->SetVisibility(ESlateVisibility::Hidden);
		}
		
		if (DamagableComponent->Mana > 0)
		{
			ManaImage->SetVisibility(ESlateVisibility::Visible);
			for (int32 i = 0; i < DamagableComponent->Mana - 1; ++i)
			{
				if (i < CreatedManaUnits.Num())
				{
					CreatedManaUnits[i]->SetVisibility(ESlateVisibility::Visible);
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, *FString::Printf(TEXT("Mana %d"), DamagableComponent->Mana));
				}
			}
		}
		else
		{
			ManaImage->SetVisibility(ESlateVisibility::Hidden);
		}

		for (UImage* EmptyMana : CreatedEmptyManaUnits)
		{
			EmptyMana->SetVisibility(ESlateVisibility::Hidden);
		}

		if (DamagableComponent->MaxMana > 0)
		{
			EmptyManaImage->SetVisibility(ESlateVisibility::Visible);
			for (int32 i = 0; i < DamagableComponent->MaxMana - 1; ++i)
			{
				if (i < CreatedEmptyManaUnits.Num())
				{
					CreatedEmptyManaUnits[i]->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
		else
		{
			EmptyManaImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UCharacterHUD::ToggleCrosshair(bool bEnabled)
{
	if (bEnabled)
	{
		if (!Crosshair->IsVisible())
		{
			Crosshair->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		Crosshair->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCharacterHUD::PlayDamageEffect_Implementation(AActor* Dealer)
{
}

void UCharacterHUD::SetVisible()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UCharacterHUD::SetHidden()
{
	SetVisibility(ESlateVisibility::Hidden);
}
