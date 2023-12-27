// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HCGameMode.generated.h"

class AFieldVisualizer;
class APlayerVisualizer;
class ACardSlot;
/**
 * 
 */
UCLASS()
class HEROICNEXUS_API AHCGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HCGameMode")
	TSubclassOf<AFieldVisualizer> VisualizerClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HCGameMode")
	TSubclassOf<APlayerVisualizer> PlayerVisualizerClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HCGameMode")
	TSubclassOf<ACardSlot> CardSlotClass = nullptr;
	
};
