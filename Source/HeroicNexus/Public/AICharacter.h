// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "AICharacter.generated.h"

UCLASS()
class HEROICNEXUS_API AAICharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PlayPhase(const EPhases Phase) override;
	virtual void SelectCards(const TArray<ACard*> CardPool, int32 Min, int32 Max) override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
