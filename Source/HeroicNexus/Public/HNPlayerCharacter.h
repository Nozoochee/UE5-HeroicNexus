// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "HNPlayerCharacter.generated.h"

UCLASS()
class HEROICNEXUS_API AHNPlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHNPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PlayPhase(const EPhases Phase) override;
	virtual void SelectCards(const TArray<ACard*> CardPool, int32 Min, int32 Max) override;

	UFUNCTION(BlueprintNativeEvent)
	void PlayPhaseBP(const EPhases Phase);
	UFUNCTION(BlueprintNativeEvent)
	void SelectCardsBP(const TArray<ACard*>& CardPool, int32 Min, int32 Max);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
