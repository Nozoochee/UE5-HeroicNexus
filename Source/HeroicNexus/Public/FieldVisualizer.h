// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayField.h"
#include "GameFramework/Actor.h"
#include "FieldVisualizer.generated.h"


class APlayerVisualizer;
class APlayField;

UENUM(Blueprintable)
enum class EVisualizationMode : uint8 
{
	NormalSpeed,
	DoubleSpeed,
	Skip, //Don't play any visualizations
};

DECLARE_DELEGATE(FOnCatchupCompleteSignature);

UCLASS()
class HEROICNEXUS_API AFieldVisualizer : public AActor
{
	GENERATED_BODY()
/*
 * General flow:
 * PlayField runs all the logic
 * When a process finishes (e.g. drawing a card), it adds it to the Visualizer
 * The Visualizer then shows what happened to the player via animations and/or UI
 * For things that only affects 1 player, it uses a PlayerVisualizer
 */
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visualizer")
	APlayField* Field = nullptr;
	UPROPERTY(BlueprintReadOnly, Category="Visualizer")
	FDuelRules Rules;
	FOnCatchupCompleteSignature OnCatchupCompleteDelegate;

protected:
	UPROPERTY(BlueprintReadOnly, Category="Visualizer")
	bool bIsVisualizing = false;
	UPROPERTY(BlueprintReadOnly, Category="Visualizer")
	TArray<FDuelOperation> VisualizeOperations;
	UPROPERTY(BlueprintReadOnly, Category="Visualizer")
	TMap<ABaseCharacter*, APlayerVisualizer*> PlayerVisualizers;
	
	
public:
	// Sets default values for this actor's properties
	AFieldVisualizer();

	UFUNCTION(BlueprintNativeEvent, Category="Visualizer")
	void StartVisualization();

	UFUNCTION(BlueprintCallable, Category="Visualizer")
	void FinishedCatchingUp();
	
	bool GetIsVisualizing() const {return bIsVisualizing;}
	void AddVisualizeOperation(const FDuelOperation& input){VisualizeOperations.Add(input);}
	int32 GetNumVisualizeOperations() const {return VisualizeOperations.Num();}

	UFUNCTION(BlueprintNativeEvent, Category="Visualizer")
	void StartHumanPlayerDuelUI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
