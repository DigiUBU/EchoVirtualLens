#include "PhotoEvalLibrary.h"
#include "Kismet/KismetMathLibrary.h"

//Calculates de score in the Photo Level (03) depending on all the parameters)
float UPhotoEvalLibrary::CalculatePhotoScore(
    FVector ActualLocation,
    FRotator ActualRotation,
    FVector TargetLocation,
    FRotator TargetRotation,
    float CaptureTime,
    float AllowedTime,
    int32 Fails,
    int32 MaxFails,
    float AlphaTime,
    float BetaFails,
    float RotationToleranceDeg,
    float LocationTolerance,
    //NEW:
    float Lambda,
    float PreviousScore
)
{
    /*OLD
    float TimeScore = 1.f - (CaptureTime / AllowedTime);
    TimeScore = FMath::Clamp(TimeScore, 0.f, 1.f);

    float FailScore = 1.f - (static_cast<float>(Fails) / static_cast<float>(MaxFails));
    FailScore = FMath::Clamp(FailScore, 0.f, 1.f);

    float DistError = FVector::Dist(ActualLocation, TargetLocation);
    float PosScore = 1.f - (DistError / LocationTolerance);
    PosScore = FMath::Clamp(PosScore, 0.f, 1.f);

    float RotError = FMath::Abs((ActualRotation - TargetRotation).GetNormalized().Yaw);
    float RotScore = 1.f - (RotError / RotationToleranceDeg);
    RotScore = FMath::Clamp(RotScore, 0.f, 1.f);

    float BaseScore = (AlphaTime * TimeScore) + (BetaFails * FailScore);

    float SpatialScore = 0.5f * PosScore + 0.5f * RotScore;

    float Score = BaseScore * SpatialScore;

    return FMath::Clamp(Score, 0.f, 1.f);
    */
    float TimeScore = 1.f - (CaptureTime / AllowedTime);
    TimeScore = FMath::Clamp(TimeScore, 0.f, 1.f);

    float FailScore = 1.f - (static_cast<float>(Fails) / static_cast<float>(MaxFails));
    FailScore = FMath::Clamp(FailScore, 0.f, 1.f);

    float DistError = FVector::Dist(ActualLocation, TargetLocation);
    float PosScore = 1.f - (DistError / LocationTolerance);
    PosScore = FMath::Clamp(PosScore, 0.f, 1.f);

    float RotError = FMath::Abs((ActualRotation - TargetRotation).GetNormalized().Yaw);
    float RotScore = 1.f - (RotError / RotationToleranceDeg);
    RotScore = FMath::Clamp(RotScore, 0.f, 1.f);

    float BaseScore = (AlphaTime * TimeScore) + (BetaFails * FailScore);

    float SpatialScore = 0.5f * PosScore + 0.5f * RotScore;

    float RawScore = BaseScore * SpatialScore;

    if (Fails >= MaxFails)
    {
        RawScore = 0.f;
    }

    float FinalScore = (Lambda * RawScore) + ((1.f - Lambda) * PreviousScore);

    return FMath::Clamp(FinalScore, 0.f, 1.f);
}

