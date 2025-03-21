#include "OceanBreathCalculator.h"
#include "Math/UnrealMathUtility.h"


TArray<float> UOceanBreathCalculator::PressureHistory;
float UOceanBreathCalculator::SmoothedPressure = 0.0f;
TArray<float> UOceanBreathCalculator::OceanPressureHistory;
float UOceanBreathCalculator::AmpDecay = 1.0f;
int UOceanBreathCalculator::DelaySteps = 0;

float UOceanBreathCalculator::CalculateOceanPressure(float HumanPressure, float CurrentTime)
{
    // ��������
    const int WindowSize = 7;                    // ƽ�����ڴ�С
    const float K_Amp = 1.2f;                    // ˥�����Ӳ���
    const float Lambda_Amp = 0.003f;             // ˥������
    const float PhaseDelay = PI / 1.3f;          // ��λ�ӳ�
    const int ConjugateOffset = 5;               // ����ƫ��
    const float RawMin = 0.0f, RawMax = 5.0f;    // ���뷶Χ
    const float MappedMin = -50.0f, MappedMax = 50.0f; // �����Χ


    auto MapRange = [](float Value, float InMin, float InMax, float OutMin, float OutMax) {
        if (InMax - InMin == 0.0f) return (OutMin + OutMax) / 2.0f;
        return (Value - InMin) / (InMax - InMin) * (OutMax - OutMin) + OutMin;
        };

    // ��ӵ�ǰ������ѹ����ʷ����
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

    // ӳ��ƽ�������ѹ
    float MappedHuman = MapRange(SmoothedPressure, RawMin, RawMax, MappedMin, MappedMax);

    // ����˥������
    AmpDecay = K_Amp * FMath::Exp(-Lambda_Amp * CurrentTime);

    // �ӳٲ������� + ��������Ŷ�
    float RandomFactor = FMath::RandRange(0.5f, 1.0f);
    DelaySteps = FMath::RoundToInt((WindowSize / (1.3f * PI)) * PhaseDelay * RandomFactor);

    // ���캣��������ߣ����ڸ��ƣ�
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

    // ע��ͻ���Գ鴤
    if (FMath::RandRange(0.0f, 1.0f) < 1.0f / 30.0f && CopiedSegment.Num() > 0)
    {
        int SpikeIndex = FMath::RandRange(0, CopiedSegment.Num() - 1);
        CopiedSegment[SpikeIndex] += FMath::RandRange(50.0f, 120.0f) * (FMath::RandBool() ? 1.0f : -1.0f);
    }

    // Ӧ�÷�ת + ƫ�� + ˥��
    float OceanPressure = (-FMath::Abs(MappedHuman) + ConjugateOffset) * AmpDecay;

    // ���º�����ѹ��ʷ
    OceanPressureHistory.Add(OceanPressure);
    if (OceanPressureHistory.Num() > WindowSize)
    {
        OceanPressureHistory.RemoveAt(0);
    }

    return OceanPressure;
}