// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BPF_HeroicNexus.generated.h"

/**
 * 
 */
UCLASS()
class HEROICNEXUS_API UBPF_HeroicNexus : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="CPPBlueprintFunctionLibrary")
	static float GetCardHandScale(){return CARD_HAND_SCALE;}
};
