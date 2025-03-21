#include "BreathRateCalculator.h"

// 静态成员变量的初始化
float UBreathRateCalculator::LastPressure = 0.0f;
TArray<float> UBreathRateCalculator::BreathTimestamps;
bool UBreathRateCalculator::IsRising = false;
float UBreathRateCalculator::SmoothedPressure = 0.0f;
TArray<float> UBreathRateCalculator::PressureHistory;

float UBreathRateCalculator::CalculateBreathRate(float CurrentPressure, float CurrentTime)
{
    // 更新呼吸频率数据
    UpdateBreathRate(CurrentPressure, CurrentTime);

    // 计算过去 60 秒内的呼吸次数 (BPM)
    BreathTimestamps.RemoveAll([CurrentTime](float Timestamp) {
        return Timestamp < CurrentTime - 60.0f;
        });
    return static_cast<float>(BreathTimestamps.Num());
}

TArray<float> UBreathRateCalculator::GetLastTenElements(const TArray<float>& InputArray)
{
    TArray<float> ResultArray;

    // 如果数组长度小于10，直接返回整个数组
    if (InputArray.Num() <= 500)
    {
        ResultArray = InputArray;
    }
    else
    {
        // 从数组末尾开始，取最后10个元素
        int32 StartIndex = InputArray.Num() - 500;
        for (int32 i = StartIndex; i < InputArray.Num(); i++)
        {
            ResultArray.Add(InputArray[i]);
        }
    }

    return ResultArray;
}

TArray<FString> UBreathRateCalculator::GetStringLastTenElements(const TArray<FString>& InputArray)
{
    TArray<FString> ResultArray;

    // 如果数组长度小于10，直接返回整个数组
    if (InputArray.Num() <= 500)
    {
        ResultArray = InputArray;
    }
    else
    {
        // 从数组末尾开始，取最后10个元素
        int32 StartIndex = InputArray.Num() - 500;
        for (int32 i = StartIndex; i < InputArray.Num(); i++)
        {
            ResultArray.Add(InputArray[i]);
        }
    }

    return ResultArray;
}
void UBreathRateCalculator::UpdateBreathRate(float CurrentPressure, float CurrentTime)
{
    // 平滑处理（使用移动平均）
    PressureHistory.Add(CurrentPressure);
    if (PressureHistory.Num() > 10) // 窗口大小为 10，可调整
    {
        PressureHistory.RemoveAt(0);
    }
    SmoothedPressure = 0.0f;
    for (float Pressure : PressureHistory)
    {
        SmoothedPressure += Pressure;
    }
    SmoothedPressure /= PressureHistory.Num();

    // 检测呼吸波峰（从上升转为下降时记录一次呼吸）
    if (SmoothedPressure < LastPressure && IsRising)
    {
        BreathTimestamps.Add(CurrentTime);
        IsRising = false;
    }
    else if (SmoothedPressure > LastPressure)
    {
        IsRising = true;
    }

    // 更新上一次气压值
    LastPressure = SmoothedPressure;
}