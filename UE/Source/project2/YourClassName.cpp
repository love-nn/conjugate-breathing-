#include "YourClassName.h"

void UYourClassName::SetPostProcessBlurIntensity(AActor* Volume, float Intensity, UMaterialInterface* Material)
{
    if (!Volume || !Material) return;

    APostProcessVolume* PostVolume = Cast<APostProcessVolume>(Volume);
    if (!PostVolume) return;

    TArray<FWeightedBlendable>& Blendables = PostVolume->Settings.WeightedBlendables.Array;
    if (Blendables.Num() == 0)
    {
        FWeightedBlendable NewBlendable;
        NewBlendable.Weight = 1.0f;
        NewBlendable.Object = Material;
        Blendables.Add(NewBlendable);
    }
    UMaterialInstanceDynamic* DynamicMat = UMaterialInstanceDynamic::Create(Material, nullptr);
    if (DynamicMat)
    {
        DynamicMat->SetScalarParameterValue(TEXT("BlurIntensity"), Intensity);
        if (Blendables.Num() > 0)
        {
            Blendables[0].Object = DynamicMat;
        }
        else
        {
            FWeightedBlendable NewBlendable;
            NewBlendable.Weight = 1.0f;
            NewBlendable.Object = DynamicMat;
            Blendables.Add(NewBlendable);
        }
        PostVolume->Settings.WeightedBlendables = FWeightedBlendables(Blendables);
    }
}