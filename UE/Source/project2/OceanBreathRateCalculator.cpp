#include "OceanBreathRateCalculator.h"

// ��̬��Ա���������ڴ洢״̬
float UOceanBreathRateCalculator::LastOceanPressure = 0.0f; // ��һ�ε���ѹֵ
TArray<float> UOceanBreathRateCalculator::OceanBreathTimestamps; // �洢����ʱ���
bool UOceanBreathRateCalculator::IsOceanRising = false; // �Ƿ�������״̬

float UOceanBreathRateCalculator::CalculateOceanBreathRate(float OceanPressure, float CurrentTime)
{
    // ��Ⲩ�壺����ѹ������תΪ�½�ʱ����¼ʱ���
    if (OceanPressure < LastOceanPressure && IsOceanRising)
    {
        OceanBreathTimestamps.Add(CurrentTime); // ��ӵ�ǰʱ���
        IsOceanRising = false; // ���Ϊ�½�
    }
    else if (OceanPressure > LastOceanPressure)
    {
        IsOceanRising = true; // ���Ϊ����
    }
    LastOceanPressure = OceanPressure; // ������һ����ѹֵ

    // �Ƴ�����60���ʱ���������������60�봰����
    OceanBreathTimestamps.RemoveAll([CurrentTime](float Timestamp) {
        return Timestamp < CurrentTime - 60.0f;
        });

    // ����г���һ�����壬���㲢����BPM
    if (OceanBreathTimestamps.Num() > 1)
    {
        // ���������������֮���ʱ����
        float LastInterval = OceanBreathTimestamps.Last() - OceanBreathTimestamps[OceanBreathTimestamps.Num() - 2];
        // ����������ڹ���BPM
        float EstimatedBPM = 60.0f / LastInterval;

        // ������ݿ�ȴﵽ�򳬹�60�룬ʹ��60���ڵĺ�������
        if (CurrentTime - OceanBreathTimestamps[0] >= 60.0f)
        {
            return static_cast<float>(OceanBreathTimestamps.Num()); // ����60���ڵĲ�������ΪBPM
        }
        // ���ݲ���60��ʱ�����ع����BPM
        else
        {
            return EstimatedBPM;
        }
    }

    // ������ݲ��㣨����2�����壩������0
    return 0.0f;
}