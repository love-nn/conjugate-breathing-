#include "OceanBreathCalculator.h"
#include "Math/UnrealMathUtility.h"


TArray<float> UOceanBreathCalculator::PressureHistory;
float UOceanBreathCalculator::SmoothedPressure = 0.0f;
TArray<float> UOceanBreathCalculator::OceanPressureHistory;
float UOceanBreathCalculator::AmpDecay = 1.0f;
int UOceanBreathCalculator::DelaySteps = 0;

float UOceanBreathCalculator::CalculateOceanPressure(float HumanPressure, float CurrentTime)
{
    // 参数设置
    const int WindowSize = 7;                    // 平滑窗口大小
    const float K_Amp = 1.2f;                    // 衰减因子参数
    const float Lambda_Amp = 0.003f;             // 衰减速率
    const float PhaseDelay = PI / 1.3f;          // 相位延迟
    const int ConjugateOffset = 5;               // 共轭偏移
    const float RawMin = 0.0f, RawMax = 5.0f;    // 输入范围
    const float MappedMin = -50.0f, MappedMax = 50.0f; // 输出范围


    auto MapRange = [](float Value, float InMin, float InMax, float OutMin, float OutMax) {
        if (InMax - InMin == 0.0f) return (OutMin + OutMax) / 2.0f;
        return (Value - InMin) / (InMax - InMin) * (OutMax - OutMin) + OutMin;
        };

    // 添加当前人类气压到历史数据
    PressureHistory.Add(HumanPressure);
    if (PressureHistory.Num() > WindowSize)
    {
        PressureHistory.RemoveAt(0);
    }


    SmoothedPressure = 0.0f;
    for (float Pressure : PressureHistory)
    {
        SmoothedPressure += Pressure;
    }
    SmoothedPressure /= PressureHistory.Num();

    // 映射平滑后的气压
    float MappedHuman = MapRange(SmoothedPressure, RawMin, RawMax, MappedMin, MappedMax);

    // 计算衰减因子
    AmpDecay = K_Amp * FMath::Exp(-Lambda_Amp * CurrentTime);

    // 延迟步数估算 + 随机周期扰动
    float RandomFactor = FMath::RandRange(0.5f, 1.0f);
    DelaySteps = FMath::RoundToInt((WindowSize / (1.3f * PI)) * PhaseDelay * RandomFactor);

    // 构造海洋呼吸曲线（周期复制）
    int ShortT = FMath::Max(3, DelaySteps / 3);
    TArray<float> Segment;
    for (int i = FMath::Max(0, PressureHistory.Num() - ShortT); i < PressureHistory.Num(); ++i)
    {
        float MappedValue = MapRange(PressureHistory[i], RawMin, RawMax, MappedMin, MappedMax);
        Segment.Add(MappedValue);
    }
    TArray<float> CopiedSegment;
    for (int i = 0; i < 3; ++i)
    {
        CopiedSegment.Append(Segment);
    }
    CopiedSegment.SetNum(PressureHistory.Num());

    // 注入突发性抽搐
    if (FMath::RandRange(0.0f, 1.0f) < 1.0f / 30.0f && CopiedSegment.Num() > 0)
    {
        int SpikeIndex = FMath::RandRange(0, CopiedSegment.Num() - 1);
        CopiedSegment[SpikeIndex] += FMath::RandRange(50.0f, 120.0f) * (FMath::RandBool() ? 1.0f : -1.0f);
    }

    // 应用反转 + 偏移 + 衰减
    float OceanPressure = (-FMath::Abs(MappedHuman) + ConjugateOffset) * AmpDecay;

    // 更新海洋气压历史
    OceanPressureHistory.Add(OceanPressure);
    if (OceanPressureHistory.Num() > WindowSize)
    {
        OceanPressureHistory.RemoveAt(0);
    }

    return OceanPressure;
}