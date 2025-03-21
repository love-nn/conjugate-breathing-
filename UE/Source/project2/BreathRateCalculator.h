#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BreathRateCalculator.generated.h"

UCLASS()
class PROJECT2_API UBreathRateCalculator : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // 蓝图可调用函数，用于计算呼吸频率 (BPM)
    UFUNCTION(BlueprintCallable, Category = "BreathRate")
    static float CalculateBreathRate(float CurrentPressure, float CurrentTime);
    UFUNCTION(BlueprintCallable, Category = "BreathRate")
    static TArray<float> GetLastTenElements(const TArray<float>& InputArray);
    UFUNCTION(BlueprintCallable, Category = "BreathRate")
    static TArray<FString> GetStringLastTenElements(const TArray<FString>& InputArray);
private:
    static float LastPressure;                   // 上一次的气压值
    static TArray<float> BreathTimestamps;       // 记录每次呼吸的时间戳
    static bool IsRising;                        // 是否处于上升阶段
    static float SmoothedPressure;               // 平滑后的气压值
    static TArray<float> PressureHistory;        // 气压历史数据，用于平滑处理

    

    static void UpdateBreathRate(float CurrentPressure, float CurrentTime);
    
};