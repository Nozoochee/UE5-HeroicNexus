// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common.generated.h"

UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ECardTypes : uint8
{
	None = 0x000,
	Monster = 0x001,
	Hero = 0x002,
	Charge = 0x004,
	Equip = 0x008,
	Flip = 0x010,
	Effect = 0x020,
	Charging = 0x040,
	Instant = 0x080,
};
ENUM_CLASS_FLAGS(ECardTypes)

UENUM(BlueprintType)
enum class ELocations : uint8
{
	None,
	Deck,
	Hand,
	MZone,
	CCZone,
	Grave,
	All
};

UENUM(BlueprintType)
enum class EReasons : uint8
{
	Rule,
	Effect,
	Battle,
	Cost,
	Draw,
	Summon,
};

UENUM(BlueprintType, meta=(Bitflags))
enum class EArchetypes
{
	None = 0x00,
};
ENUM_CLASS_FLAGS(EArchetypes)

UENUM(BlueprintType)
enum class ECardActions : uint8
{
	Inspect,
	Summon,
	Set,
	Activate,
	Flip,
	ChangePosition,
	Attack,
	Tribute,
};

USTRUCT(BlueprintType, meta=(ScriptName = "CardActionsArray"))
struct FCardActions
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ECardActions> Actions;	
};

USTRUCT(BlueprintType)
struct FDuelRules
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StartLP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StartHandCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DrawCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HandMaxCardCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StartMana;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxMana;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ManaPerTurn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinDeckSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxDeckSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MonsterZones;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CCZones;

	FDuelRules()
	{
		StartLP = 2000;
		StartHandCount = 4;
		DrawCount = 1;
		HandMaxCardCount = 6;
		StartMana = 2;
		MaxMana = 12;
		ManaPerTurn = 1;
		MinDeckSize = 8;
		MaxDeckSize = 20;
		MonsterZones = 3;
		CCZones = 3;
	}
};

constexpr float CARD_TOURNAMENT_SCALE = 0.4f;
constexpr float CARD_STREET_DUEL_SCALE = 0.25f;
constexpr float CARD_HAND_SCALE = 0.02f;

/**
 * 
 */
class HEROICNEXUS_API Common
{
	Common();
	~Common();
};
