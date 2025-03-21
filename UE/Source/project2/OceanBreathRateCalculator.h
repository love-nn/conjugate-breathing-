
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OceanBreathRateCalculator.generated.h"

UCLASS()
class PROJECT2_API UOceanBreathRateCalculator : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // ��ͼ�ɵ��ú��������ڼ��㺣�����Ƶ�� (BPM)
    UFUNCTION(BlueprintCallable, Category = "OceanBreathRate")
    static float CalculateOceanBreathRate(float OceanPressure, float CurrentTime);

private:
    static float LastOceanPressure;              // ��һ�εĺ�����ѹֵ
    static TArray<float> OceanBreathTimestamps;  // ��¼ÿ�κ��������ʱ���
    static bool IsOceanRising;                   // ������ѹ�Ƿ��������׶�
};
