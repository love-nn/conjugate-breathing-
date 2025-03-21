#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/PostProcessVolume.h"
#include "YourClassName.generated.h"

UCLASS()
class PROJECT2_API UYourClassName : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "PostProcess")
    static void SetPostProcessBlurIntensity(AActor* Volume, float Intensity, UMaterialInterface* Material);


};
