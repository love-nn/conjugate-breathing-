
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OceanBreathRateCalculator.generated.h"

UCLASS()
class PROJECT2_API UOceanBreathRateCalculator : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // 蓝图可调用函数，用于计算海洋呼吸频率 (BPM)
    UFUNCTION(BlueprintCallable, Category = "OceanBreathRate")
    static float CalculateOceanBreathRate(float OceanPressure, float CurrentTime);

private:
    static float LastOceanPressure;              // 上一次的海洋气压值
    static TArray<float> OceanBreathTimestamps;  // 记录每次海洋呼吸的时间戳
    static bool IsOceanRising;                   // 海洋气压是否处于上升阶段
};
