#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonFileHelper.generated.h"

UCLASS()
class THEVIRTUALCODE_API UJsonFileHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    //Saves a string in a file in Saved/Ranking
    UFUNCTION(BlueprintCallable, Category = "Game|SaveLoad")
    static bool SaveStringToFile(const FString& Data, const FString& FileName);

    //Loads a string from a file in Saved/Ranking
    UFUNCTION(BlueprintCallable, Category = "Game|SaveLoad")
    static bool LoadStringFromFile(const FString& FileName, FString& OutData);
};
