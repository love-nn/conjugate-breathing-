#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OceanBreathCalculator.generated.h"

UCLASS()
class PROJECT2_API UOceanBreathCalculator : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // ��ͼ�ɵ��ú��������ڼ��㺣�������ѹ
    UFUNCTION(BlueprintCallable, Category = "OceanBreath")
    static float CalculateOceanPressure(float HumanPressure, float CurrentTime);

private:
    static TArray<float> PressureHistory;        // ������ѹ��ʷ����
    static float SmoothedPressure;               // ƽ�������ѹֵ
    static TArray<float> OceanPressureHistory;   // ������ѹ��ʷ����
    static float AmpDecay;                       // ˥������
    static int DelaySteps;                       // �ӳٲ���
};