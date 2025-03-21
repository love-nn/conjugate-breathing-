#include "BreathRateCalculator.h"

// ��̬��Ա�����ĳ�ʼ��
float UBreathRateCalculator::LastPressure = 0.0f;
TArray<float> UBreathRateCalculator::BreathTimestamps;
bool UBreathRateCalculator::IsRising = false;
float UBreathRateCalculator::SmoothedPressure = 0.0f;
TArray<float> UBreathRateCalculator::PressureHistory;

float UBreathRateCalculator::CalculateBreathRate(float CurrentPressure, float CurrentTime)
{
    // ���º���Ƶ������
    UpdateBreathRate(CurrentPressure, CurrentTime);

    // �����ȥ 60 ���ڵĺ������� (BPM)
    BreathTimestamps.RemoveAll([CurrentTime](float Timestamp) {
        return Timestamp < CurrentTime - 60.0f;
        });
    return static_cast<float>(BreathTimestamps.Num());
}

TArray<float> UBreathRateCalculator::GetLastTenElements(const TArray<float>& InputArray)
{
    TArray<float> ResultArray;

    // ������鳤��С��10��ֱ�ӷ�����������
    if (InputArray.Num() <= 500)
    {
        ResultArray = InputArray;
    }
    else
    {
        // ������ĩβ��ʼ��ȡ���10��Ԫ��
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

    // ������鳤��С��10��ֱ�ӷ�����������
    if (InputArray.Num() <= 500)
    {
        ResultArray = InputArray;
    }
    else
    {
        // ������ĩβ��ʼ��ȡ���10��Ԫ��
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
    // ƽ������ʹ���ƶ�ƽ����
    PressureHistory.Add(CurrentPressure);
    if (PressureHistory.Num() > 10) // ���ڴ�СΪ 10���ɵ���
    {
        PressureHistory.RemoveAt(0);
    }
    SmoothedPressure = 0.0f;
    for (float Pressure : PressureHistory)
    {
        SmoothedPressure += Pressure;
    }
    SmoothedPressure /= PressureHistory.Num();

    // ���������壨������תΪ�½�ʱ��¼һ�κ�����
    if (SmoothedPressure < LastPressure && IsRising)
    {
        BreathTimestamps.Add(CurrentTime);
        IsRising = false;
    }
    else if (SmoothedPressure > LastPressure)
    {
        IsRising = true;
    }

    // ������һ����ѹֵ
    LastPressure = SmoothedPressure;
}