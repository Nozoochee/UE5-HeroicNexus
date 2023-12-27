// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HeroicNexus/Common.h"
#include "UObject/Object.h"
#include "CardEffect.generated.h"

class APlayField;
class ABaseCharacter;
class ACard;

UENUM(BlueprintType)
enum class EEffectType : uint8
{
	SelfEffect,
	OptionalActivate,
	MandatoryActivate,
};

UENUM(BlueprintType)
enum class EEffectTrigger : uint8
{
	Auto,
	SummonSuccess,
	Destroyed,
	Flip,
	Counter,
	Move,
	Draw,
	ToHand,
	ChangeAtk,
	ChangeDef,
	MainPhaseStart,
	BattlePhaseStart,
	FlipSummonSuccess,
	ChangePosition,
	ActivateEffect,
};

UENUM(BlueprintType)
enum class EExecuteResult : uint8
{
	Finished,
	Failed,
	WaitingForInput,
};

DECLARE_DELEGATE(FOnExecuteFinishedSignature);

/**
 * A UObject representing a single effect. The effect can permanently apply to the card itself (e.g. Taunt)
 * or do something to other cards based on it's Trigger timing
 */
UCLASS(Blueprintable)
class HEROICNEXUS_API UCardEffect : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardEffect")
	EEffectType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardEffect")
	EEffectTrigger Trigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardEffect")
	ELocations Range;

	UPROPERTY(BlueprintReadWrite, Category="CardEffect", meta=(ExposeOnSpawn=true))
	ACard* OwnerCard;

	UPROPERTY(BlueprintReadWrite, Category="CardEffect")
	ABaseCharacter* TargetPlayer;

	UPROPERTY(BlueprintReadWrite, Category="CardEffect")
	TArray<ACard*> Targets;

	UPROPERTY(BlueprintReadWrite, Category="CardEffect", meta=(ExposeOnSpawn=true))
	APlayField* Field;
	
	FOnExecuteFinishedSignature OnExecuteFinishedDelegate;

public:
	UFUNCTION(BlueprintNativeEvent, Category="CardEfect")
	bool IsActivatable();

	//Any conditions that may need to be necessary for activation
	UFUNCTION(BlueprintNativeEvent, Category="CardEfect")
	bool Condition();

	//Determines valid targets and stores them in Targets and TargetPlayer
	UFUNCTION(BlueprintNativeEvent, Category="CardEfect")
	bool Target();

	// For costs such as life point costs
	UFUNCTION(BlueprintNativeEvent, Category="CardEfect")
	bool Cost();

	UFUNCTION(BlueprintNativeEvent, Category="CardEfect")
	bool FilterTarget(ACard* Target);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="CardEfect")
	EExecuteResult Execute();
};
