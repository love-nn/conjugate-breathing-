#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BreathDataProcessor.generated.h"

UCLASS()
class PROJECT2_API UBreathDataProcessor : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "BreathData")
    static FString CalculateBreathData(float RawValue, float CurrentTime);

private:
    static TArray<float> RawData;
    static TArray<float> SmoothedData;
    static TArray<float> MappedHumanData;
    static TArray<float> MappedOceanData;
    static float LastValue;
    static float LastTime;
    static int32 SampleCount;
};