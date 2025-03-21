#include "BreathDataProcessor.h"
#include "Math/UnrealMathUtility.h"

// ��̬������ʼ��
TArray<float> UBreathDataProcessor::RawData;
TArray<float> UBreathDataProcessor::SmoothedData;
TArray<float> UBreathDataProcessor::MappedHumanData;
TArray<float> UBreathDataProcessor::MappedOceanData;
float UBreathDataProcessor::LastValue = 0.0f;
float UBreathDataProcessor::LastTime = 0.0f;
int32 UBreathDataProcessor::SampleCount = 0;

FString UBreathDataProcessor::CalculateBreathData(float RawValue, float CurrentTime)
{
    // �������ã��� Python һ�£�
    const int32 WindowSize = 7;
    const int32 PolyOrder = 2;
    const float KAmp = 1.2f;
    const float LambdaAmp = 0.00015f;
    const float PhaseDelay = PI / 1.3f;
    const float ConjugateOffset = 5.0f;
    const float RawMin = 0.0f, RawMax = 5.0f;
    const float MappedMin = -50.0f, MappedMax = 50.0f;

    // ��ʼ��ʱ��
    if (LastTime == 0.0f)
    {
        LastTime = CurrentTime;
    }

    // �洢ԭʼ����
    RawData.Add(RawValue);
    SampleCount++;

    // ƽ���˲���Savitzky-Golay �򻯰棩
    if (RawData.Num() >= WindowSize)
    {
        // ���ƶ�ƽ������� Savitzky-Golay �˲���
        float Sum = 0.0f;
        for (int32 i = RawData.Num() - WindowSize; i < RawData.Num(); i++)
        {
            Sum += RawData[i];
        }
        float SmoothedValue = Sum / WindowSize;
        SmoothedData.Add(SmoothedValue);

        // �Ƴ������ݣ����ִ��ڴ�С
        if (SmoothedData.Num() > WindowSize)
        {
            SmoothedData.RemoveAt(0);
            RawData.RemoveAt(0);
        }
    }
    else
    {
        SmoothedData.Add(RawValue);
    }

    // ӳ�䵽���������Χ [-50, 50]
    float MappedHumanValue = 0.0f;
    if (RawMax - RawMin != 0)
    {
        MappedHumanValue = (SmoothedData.Last() - RawMin) / (RawMax - RawMin) * (MappedMax - MappedMin) + MappedMin;
    }
    else
    {
        MappedHumanValue = (MappedMin + MappedMax) / 2.0f;
    }
    MappedHumanData.Add(MappedHumanValue);

    // ������������߼�
    float AmpDecay = KAmp * FMath::Exp(-LambdaAmp * SampleCount);
    float RandomFactor = FMath::RandBool() ? 1.0f : 0.5f;
    int32 DelaySteps = FMath::RoundToInt((WindowSize / (1.3f * PI)) * PhaseDelay * RandomFactor);
    int32 ShortT = FMath::Max(3, DelaySteps / 3);

    // ��ȡ����� ShortT ������
    TArray<float> Segment;
    int32 StartIndex = FMath::Max(0, MappedHumanData.Num() - ShortT);
    for (int32 i = StartIndex; i < MappedHumanData.Num(); i++)
    {
        Segment.Add(MappedHumanData[i]);
    }

    // ���� Segment ����
    TArray<float> CopiedSegment;
    for (int32 i = 0; i < 3; i++)
    {
        CopiedSegment.Append(Segment);
    }
    // �ü��� MappedHumanData �ĳ���
    while (CopiedSegment.Num() > MappedHumanData.Num())
    {
        CopiedSegment.RemoveAt(CopiedSegment.Num() - 1);
    }

    // ����Ŷ�
    if (FMath::RandRange(0.0f, 1.0f) < 1.0f / 30.0f && CopiedSegment.Num() > 0)
    {
        int32 SpikeIndex = FMath::RandRange(0, CopiedSegment.Num() - 1);
        float SpikeValue = FMath::RandBool() ? 1.0f : -1.0f;
        CopiedSegment[SpikeIndex] += SpikeValue * FMath::RandRange(50.0f, 120.0f);
    }

    // ���� MappedOcean
    TArray<float> TempMappedOcean;
    for (float Value : CopiedSegment)
    {
        float OceanValue = ((-FMath::Abs(Value) + ConjugateOffset) * AmpDecay) * 10.0f;
        TempMappedOcean.Add(OceanValue);
    }
    MappedOceanData = TempMappedOcean;

    // ���㵼��
    float Dt = CurrentTime - LastTime;
    float Derivative = 0.0f;
    if (Dt > 0 && MappedOceanData.Num() > 0)
    {
        Derivative = (MappedOceanData.Last() - LastValue) / Dt;
    }
    LastValue = MappedOceanData.Num() > 0 ? MappedOceanData.Last() : 0.0f;
    LastTime = CurrentTime;

    // ��ʽ������ַ���
    float MappedOcean = MappedOceanData.Num() > 0 ? MappedOceanData.Last() : 0.0f;
    FString OutStr = FString::Printf(TEXT("%.2f|%.2f\n"), MappedOcean, Derivative);
    return OutStr;
}