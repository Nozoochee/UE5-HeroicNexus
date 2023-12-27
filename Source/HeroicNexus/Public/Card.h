// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CardEffect.h"
#include "Engine/DataTable.h"
#include "HeroicNexus/Common.h"
#include "Card.generated.h"

class UCardEffect;
class APlayField;
class ABaseCharacter;
class UTexture;
class USkeletalMesh;

UENUM(BlueprintType)
enum class ECardPosition : uint8
{
	FaceupAttack,
	FaceupDefense,
	Set,
};

UENUM(meta=(Bitflags))
enum class ECardAttributes : uint32
{
	None = 0x0000,
	CannotAttack = 0x0001,
	CannotActivate = 0x0002,
	Disable = 0x0004,
	CannotDisable = 0x0008,
	CannotSummon = 0x0010,
	Indestructible = 0x0020,
	IndestructibleByBattle = 0x0040,
	IndestructibleByEffect = 0x0080,
	Taunt = 0x0100,
	Piercing = 0x0200,
	CannotTribute = 0x0400,
};
ENUM_CLASS_FLAGS(ECardAttributes)

UENUM(BlueprintType, meta=(Bitflags))
enum class ECardStatuses
{
	None = 0x00,
	SummonTurn = 0x01,
	FlipSummonTurn = 0x02,
	SetTurn = 0x04,
	PosChanged = 0x08,
	EffectDisabled = 0x10,
};
ENUM_CLASS_FLAGS(ECardStatuses)

//Original data from the database
USTRUCT(BlueprintType)
struct FCardData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Atk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Def;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Archetype;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	FCardData()
	{
		Id = 0;
		Type = 0;
		Atk = 0;
		Def = 0;
		Cost = 0;
		Archetype = 0;
		Name = FText::FromString("");
		Description = FText::FromString("");
	}
};

//Used inside duels
USTRUCT(BlueprintType)
struct FCardState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Bitmask, BitmaskEnum="/Script/HeroicNexus.ECardTypes"))
	int32 Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Atk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BaseAtk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Def;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BaseDef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Charge;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Controller;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELocations Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardPosition Position;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EReasons Reason;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABaseCharacter* ReasonPlayer;

	FCardState()
	{
		Id = 0;
		Type = 0;
		Atk = 0;
		BaseAtk = 0;
		Def = 0;
		BaseDef = 0;
		Cost = 0;
		Charge = 0;
		Controller = nullptr;
		Location = ELocations::Deck;
		Position = ECardPosition::Set;
		Reason = EReasons::Rule;
		ReasonPlayer = nullptr;
	}
};

USTRUCT(BlueprintType)
struct FToFieldParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABaseCharacter* MovePlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABaseCharacter* Player;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELocations Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardPosition Position;

	FToFieldParams()
	{
		MovePlayer = nullptr;
		Player = nullptr;
		Location = ELocations::Deck;
		Position = ECardPosition::Set;
	}
};

USTRUCT(BlueprintType)
struct FCardArtTable : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture> CardArt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USkeletalMesh> Mesh;

	FCardArtTable()
	{
		CardArt = nullptr;
		Mesh = nullptr;
	}
};

UCLASS()
class HEROICNEXUS_API ACard : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardData")
	int32 Id;
	UPROPERTY(BlueprintReadWrite, Category="CardData")
	FCardData OriginalData;
	UPROPERTY(BlueprintReadWrite, Category="CardData")
	FCardState CurrentState;
	UPROPERTY(BlueprintReadWrite, Category="CardData")
	FCardState PreviousState;
	UPROPERTY(BlueprintReadWrite, Category="CardData", meta=(Bitmask, BitmaskEnum="/Script/HeroicNexus.ECardStatuses"))
	int32 Statuses = 0;
	UPROPERTY(BlueprintReadWrite, Category="CardData")
	int32 AttackCount = 1;		
	UPROPERTY(BlueprintReadWrite, Category="CardData")
	int32 EffectActivationCount = 1;
	UPROPERTY(BlueprintReadWrite, Category="CardData")
	int32 ChangePositionCount = 1;
	UPROPERTY(BlueprintReadWrite, Category="CardData", meta=(Bitmask, BitmaskEnum="/Script/HeroicNexus.ECardAttributes"))
	int32 AppliedAttributes;
	UPROPERTY(BlueprintReadWrite, Category="CardData")
	FToFieldParams ToFieldParams;
	UPROPERTY(BlueprintReadWrite, Category="CardData")
	APlayField* Field;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CardData")
	TArray<TSubclassOf<UCardEffect>> BaseEffects;
	UPROPERTY(BlueprintReadWrite, Category="CardData")
	TArray<UCardEffect*> AvailableEffects;
	
public:
	// Sets default values for this actor's properties
	ACard();

	UFUNCTION(BlueprintCallable, Category="Card")
	void ResetTurnAttributes();
	UFUNCTION(BlueprintCallable, Category="Card")
	TArray<UCardEffect*> GetTriggerableEffects(const EEffectTrigger Trigger);
	UFUNCTION(BlueprintCallable)
	TArray<ECardActions> GetAllUsableActions(ABaseCharacter* Player);
	UFUNCTION(BlueprintCallable, Category="Card")
	void IncreaseUseCount(const int32 Count = 1);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION(Blueprintable, Category="Card")
	void RegisterEffect(UCardEffect* Effect);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Card")
	bool CanAttack() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Card")
	bool CanBeSummoned() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Card")
	bool CanBeSet() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Card")
	bool CanActivate() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Card")
	bool CanFlip() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Card")
	bool CanChangePosition() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Card")
	bool CanBeTributed() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Card")
	bool HasAttribute(ECardAttributes Attribute) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Card")
	bool HasStatus(ECardStatuses Status) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Card")
	bool IsType(ECardTypes Type) const;
};
