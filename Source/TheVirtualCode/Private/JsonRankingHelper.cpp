#include "JsonRankingHelper.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"


//The data to save in the ranking
bool UJsonRankingHelper::SaveRankingToJson(const FString& FileName, const TArray<FParsedEntry>& Entries)
{
    TArray<TSharedPtr<FJsonValue>> JsonArray;

    for (const FParsedEntry& E : Entries)
    {
        TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
        Obj->SetStringField(TEXT("Jugador"), E.PlayerName);
        Obj->SetNumberField(TEXT("Nivel"), E.Level);
        Obj->SetNumberField(TEXT("ScoreNivel"), E.Score);
        Obj->SetNumberField(TEXT("TiempoNivel"), E.Time);
        Obj->SetStringField(TEXT("SessionId"), E.SessionId);
        Obj->SetNumberField(TEXT("ScoreTotal"), E.ScoreTotal);
        Obj->SetNumberField(TEXT("TiempoTotal"), E.TimeTotal);

        JsonArray.Add(MakeShared<FJsonValueObject>(Obj));
    }

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonArray, Writer);

    FString FullPath = FPaths::ProjectSavedDir() / FileName;
    return FFileHelper::SaveStringToFile(OutputString, *FullPath);
}

//What is loaded from JSON
bool UJsonRankingHelper::LoadRankingFromJson(const FString& FileName, TArray<FParsedEntry>& OutEntries)
{
    OutEntries.Empty();
    FString FullPath = FPaths::ProjectSavedDir() / FileName;
    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *FullPath))
    {
        return false;
    }

    TSharedPtr<FJsonValue> RootValue;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (!FJsonSerializer::Deserialize(Reader, RootValue) || !RootValue.IsValid())
    {
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>> JsonArray = RootValue->AsArray();

    for (const TSharedPtr<FJsonValue>& Val : JsonArray)
    {
        TSharedPtr<FJsonObject> Obj = Val->AsObject();
        if (!Obj.IsValid()) continue;

        FParsedEntry Entry;
        Entry.PlayerName = Obj->GetStringField(TEXT("Jugador"));
        Entry.Level = (int32)Obj->GetNumberField(TEXT("Nivel"));
        Entry.Score = (int32)Obj->GetNumberField(TEXT("ScoreNivel"));
        Entry.Time = (float)Obj->GetNumberField(TEXT("TiempoNivel"));
        Entry.SessionId = Obj->GetStringField(TEXT("SessionId"));
        Entry.ScoreTotal = (int32)Obj->GetNumberField(TEXT("ScoreTotal"));
        Entry.TimeTotal = (float)Obj->GetNumberField(TEXT("TiempoTotal"));

        OutEntries.Add(Entry);
    }

    return true;
}

//The best in 1 level (chosen in parameters and also the amount of users to show)
void UJsonRankingHelper::GetTopNByLevel(const FString& FileName, int32 Level, int32 N, TArray<FParsedEntry>& OutTop)
{
    TArray<FParsedEntry> AllEntries;
    LoadRankingFromJson(FileName, AllEntries);

    TArray<FParsedEntry> Filtered;
    for (const FParsedEntry& E : AllEntries)
    {
        if (E.Level == Level)
        {
            Filtered.Add(E);
        }
    }

    Filtered.Sort([](const FParsedEntry& A, const FParsedEntry& B)
        {
            return A.Score > B.Score;
        });

    for (int32 i = 0; i < FMath::Min(N, Filtered.Num()); i++)
    {
        OutTop.Add(Filtered[i]);
    }
}

//The best in the whole game session (number of users to shown in parameters)
void UJsonRankingHelper::GetTopNSessions(const FString& FileName, int32 N, TArray<FSessionSummary>& OutTopSessions)
{
    TArray<FParsedEntry> AllEntries;
    LoadRankingFromJson(FileName, AllEntries);

    TMap<FString, FSessionSummary> SessionMap;
    for (const FParsedEntry& E : AllEntries)
    {
        if (!SessionMap.Contains(E.SessionId))
        {
            FSessionSummary S;
            S.PlayerName = E.PlayerName;
            S.SessionId = E.SessionId;
            S.ScoreTotal = E.ScoreTotal;
            S.TimeTotal = E.TimeTotal;
            SessionMap.Add(E.SessionId, S);
        }
    }

    TArray<FSessionSummary> Sessions;
    SessionMap.GenerateValueArray(Sessions);

    Sessions.Sort([](const FSessionSummary& A, const FSessionSummary& B)
        {
            return A.ScoreTotal > B.ScoreTotal;
        });

    for (int32 i = 0; i < FMath::Min(N, Sessions.Num()); i++)
    {
        OutTopSessions.Add(Sessions[i]);
    }
}
