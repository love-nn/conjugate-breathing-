#include "OceanBreathRateCalculator.h"

// 静态成员变量，用于存储状态
float UOceanBreathRateCalculator::LastOceanPressure = 0.0f; // 上一次的气压值
TArray<float> UOceanBreathRateCalculator::OceanBreathTimestamps; // 存储波峰时间戳
bool UOceanBreathRateCalculator::IsOceanRising = false; // 是否处于上升状态

float UOceanBreathRateCalculator::CalculateOceanBreathRate(float OceanPressure, float CurrentTime)
{
    // 检测波峰：当气压从上升转为下降时，记录时间戳
    if (OceanPressure < LastOceanPressure && IsOceanRising)
    {
        OceanBreathTimestamps.Add(CurrentTime); // 添加当前时间戳
        IsOceanRising = false; // 标记为下降
    }
    else if (OceanPressure > LastOceanPressure)
    {
        IsOceanRising = true; // 标记为上升
    }
    LastOceanPressure = OceanPressure; // 更新上一次气压值

    // 移除超过60秒的时间戳，保持数据在60秒窗口内
    OceanBreathTimestamps.RemoveAll([CurrentTime](float Timestamp) {
        return Timestamp < CurrentTime - 60.0f;
        });

    // 如果有超过一个波峰，计算并返回BPM
    if (OceanBreathTimestamps.Num() > 1)
    {
        // 计算最近两个波峰之间的时间间隔
        float LastInterval = OceanBreathTimestamps.Last() - OceanBreathTimestamps[OceanBreathTimestamps.Num() - 2];
        // 根据最近周期估算BPM
        float EstimatedBPM = 60.0f / LastInterval;

        // 如果数据跨度达到或超过60秒，使用60秒内的呼吸次数
        if (CurrentTime - OceanBreathTimestamps[0] >= 60.0f)
        {
            return static_cast<float>(OceanBreathTimestamps.Num()); // 返回60秒内的波峰数作为BPM
        }
        // 数据不足60秒时，返回估算的BPM
        else
        {
            return EstimatedBPM;
        }
    }

    // 如果数据不足（少于2个波峰），返回0
    return 0.0f;
}