#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OceanBreathCalculator.generated.h"

UCLASS()
class PROJECT2_API UOceanBreathCalculator : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // 蓝图可调用函数，用于计算海洋呼吸气压
    UFUNCTION(BlueprintCallable, Category = "OceanBreath")
    static float CalculateOceanPressure(float HumanPressure, float CurrentTime);

private:
    static TArray<float> PressureHistory;        // 人类气压历史数据
    static float SmoothedPressure;               // 平滑后的气压值
    static TArray<float> OceanPressureHistory;   // 海洋气压历史数据
    static float AmpDecay;                       // 衰减因子
    static int DelaySteps;                       // 延迟步数
};