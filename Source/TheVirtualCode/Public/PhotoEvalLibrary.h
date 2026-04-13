// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PhotoEvalLibrary.generated.h"

//Calculates score of Photo Level 03
UCLASS()
class THEVIRTUALCODE_API UPhotoEvalLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    //Calculates score with a margin and modifying the tolerance
    UFUNCTION(BlueprintCallable, Category = "PhotoEval")
    static float CalculatePhotoScore(
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
        float RotationToleranceDeg = 10.f,
        float LocationTolerance = 50.f,
        //NEW for lambda function:
        float Lambda = 0.65f,
        float PreviousScore = 0.0f
    );
	
};
