#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonRankingHelper.generated.h"

//STRUCTS:

USTRUCT(BlueprintType)
struct FParsedEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite) FString PlayerName;
    UPROPERTY(BlueprintReadWrite) int32 Level = 0;
    UPROPERTY(BlueprintReadWrite) int32 Score = 0;
    UPROPERTY(BlueprintReadWrite) float Time = 0.f;
    UPROPERTY(BlueprintReadWrite) FString SessionId;
    UPROPERTY(BlueprintReadWrite) int32 ScoreTotal = 0;
    UPROPERTY(BlueprintReadWrite) float TimeTotal = 0.f;
};

USTRUCT(BlueprintType)
struct FSessionSummary
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite) FString PlayerName;
    UPROPERTY(BlueprintReadWrite) FString SessionId;
    UPROPERTY(BlueprintReadWrite) int32 ScoreTotal = 0;
    UPROPERTY(BlueprintReadWrite) float TimeTotal = 0.f;
};

//CLASS:
//Default

UCLASS()
class THEVIRTUALCODE_API UJsonRankingHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    //Save ranking to JSON
    UFUNCTION(BlueprintCallable, Category = "Ranking")
    static bool SaveRankingToJson(const FString& FileName, const TArray<FParsedEntry>& Entries);

    //Load ranking from JSON
    UFUNCTION(BlueprintCallable, Category = "Ranking")
    static bool LoadRankingFromJson(const FString& FileName, TArray<FParsedEntry>& OutEntries);

    //Who are the best N players in a level?
    UFUNCTION(BlueprintCallable, Category = "Ranking")
    static void GetTopNByLevel(const FString& FileName, int32 Level, int32 N, TArray<FParsedEntry>& OutTop);

    //Who are the best N players in the game session?
    UFUNCTION(BlueprintCallable, Category = "Ranking")
    static void GetTopNSessions(const FString& FileName, int32 N, TArray<FSessionSummary>& OutTopSessions);
};
