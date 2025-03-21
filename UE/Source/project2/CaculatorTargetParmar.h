// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CaculatorTargetParmar.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT2_API UCaculatorTargetParmar : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "ParamCaculator")
	static float CalculateTargetParam(float CurBreathRate, float CurParam, float MinVal, float MaxVal);
	UFUNCTION(BlueprintCallable, Category = "ParamCaculator")
	static float CalculateNewParam(float CurParam, float TarParam, float inc);
	UFUNCTION(BlueprintCallable, Category = "ParamCaculator")
	static bool IsVaild(float CurParam, float Min, float Max);
};
