// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HeroicNexus/Common.h"
#include "PlayerVisualizer.generated.h"

class AHCGameMode;
class ACardSlot;
class ABaseCharacter;
/**
 * Visualizes a singular player's half of the field, their hand, deck, etc
 */
UCLASS()
class HEROICNEXUS_API APlayerVisualizer : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FDuelRules Rules;
	UPROPERTY(BlueprintReadOnly)
	ABaseCharacter* PlayerRef = nullptr;

protected:
	UPROPERTY(BlueprintReadOnly)
	AHCGameMode* GameMode = nullptr;
	
public:
	// Sets default values for this actor's properties
	APlayerVisualizer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
