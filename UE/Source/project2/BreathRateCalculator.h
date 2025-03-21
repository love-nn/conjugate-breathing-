#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BreathRateCalculator.generated.h"

UCLASS()
class PROJECT2_API UBreathRateCalculator : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // ��ͼ�ɵ��ú��������ڼ������Ƶ�� (BPM)
    UFUNCTION(BlueprintCallable, Category = "BreathRate")
    static float CalculateBreathRate(float CurrentPressure, float CurrentTime);
    UFUNCTION(BlueprintCallable, Category = "BreathRate")
    static TArray<float> GetLastTenElements(const TArray<float>& InputArray);
    UFUNCTION(BlueprintCallable, Category = "BreathRate")
    static TArray<FString> GetStringLastTenElements(const TArray<FString>& InputArray);
private:
    static float LastPressure;                   // ��һ�ε���ѹֵ
    static TArray<float> BreathTimestamps;       // ��¼ÿ�κ�����ʱ���
    static bool IsRising;                        // �Ƿ��������׶�
    static float SmoothedPressure;               // ƽ�������ѹֵ
    static TArray<float> PressureHistory;        // ��ѹ��ʷ���ݣ�����ƽ������

    

    static void UpdateBreathRate(float CurrentPressure, float CurrentTime);
    
};