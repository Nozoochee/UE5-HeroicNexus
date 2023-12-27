// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HeroicNexus/Common.h"
#include "Duel.generated.h"

class AFieldVisualizer;
class APlayField;
class ABaseCharacter;

UENUM(BlueprintType)
enum class EDuelState : uint8
{
	None,
	Waiting,
	Dueling,
	Win,
	Lose,
	Draw,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDuelStartedSignature);

UCLASS()
class HEROICNEXUS_API ADuel : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Duel", meta=(ExposeOnSpawn))
	TArray<ABaseCharacter*> Players;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Duel")
	APlayField* MainField = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Duel")
	AFieldVisualizer* Visualizer = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Duel", meta=(ExposeOnSpawn))
	FDuelRules Rules;
	UPROPERTY(BlueprintAssignable, Category="Duel")
	FOnDuelStartedSignature OnDuelStartedDelegate;
	
public:
	// Sets default values for this actor's properties
	ADuel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void StartDuel();

public:
	// Called every frame
	//virtual void Tick(float DeltaTime) override;
};
