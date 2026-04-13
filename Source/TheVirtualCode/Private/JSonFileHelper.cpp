#include "JsonFileHelper.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

//Save to file the data from ranking (creates it if it does not exist)
bool UJsonFileHelper::SaveStringToFile(const FString& Data, const FString& FileName)
{
    FString FullPath = FPaths::ProjectSavedDir() / TEXT("Ranking") / FileName;

    IFileManager::Get().MakeDirectory(*FPaths::GetPath(FullPath), true);

    return FFileHelper::SaveStringToFile(Data, *FullPath);
}

//Looks for the ranking file and loads the content
bool UJsonFileHelper::LoadStringFromFile(const FString& FileName, FString& OutData)
{
    FString FullPath = FPaths::ProjectSavedDir() / TEXT("Ranking") / FileName;

    return FFileHelper::LoadFileToString(OutData, *FullPath);
}
