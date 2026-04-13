// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpSenderActor.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/Base64.h"

//Default
AHttpSenderActor::AHttpSenderActor()
{
 	PrimaryActorTick.bCanEverTick = true;

}

void AHttpSenderActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHttpSenderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Send 1 .txt file

void AHttpSenderActor::SendTxtFile(const FString& FilePath)
{
    FString FileContent;
    if (!FFileHelper::LoadFileToString(FileContent, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("TXT file could not be read: %s"), *FilePath);
        return;
    }

    //Google App Script
    FString Url = TEXT("https://script.google.com/macros/s/AKfycbw-C1BO7_1osjGQHCdTjnkxtwOuuF4Yem8wS2dkDEm3V7qnTy7JbhfUAgrCB7NBU7Z1/exec");

    //JSON with the data to send
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField("token", "MI_TOKEN_SECRETO"); //script
    JsonObject->SetStringField("filename", FPaths::GetCleanFilename(FilePath));
    JsonObject->SetStringField("content", FileContent);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    //HTTP post request
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &AHttpSenderActor::OnResponseReceived);
    Request->SetURL(Url);
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "application/json");
    Request->SetContentAsString(OutputString);

    Request->ProcessRequest();
}

//Send ALL .txt files it can find in the folder Data

void AHttpSenderActor::SendAllTxtFiles()
{
    FString Directory = FPaths::ProjectSavedDir() / TEXT("Data");

    //File manager
    IFileManager& FileManager = IFileManager::Get();

    //Array to save the list of .txt files
    TArray<FString> Files;
    FileManager.FindFiles(Files, *(Directory / TEXT("*.txt")), true, false);

    if (Files.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("TXT files could not be found: %s"), *Directory);
        return;
    }
    
    //Array with all files together to be sent in the same email
    TArray<TSharedPtr<FJsonValue>> FileArray;

    for (const FString& FileName : Files)
    {
        FString FilePath = Directory / FileName;
        FString FileContent;
        if (FFileHelper::LoadFileToString(FileContent, *FilePath))
        {
            TSharedPtr<FJsonObject> FileObject = MakeShareable(new FJsonObject);
            FileObject->SetStringField("filename", FileName);
            FileObject->SetStringField("content", FileContent);
            FileArray.Add(MakeShareable(new FJsonValueObject(FileObject)));
        }
    }
    
    //Current date and time
    FDateTime Now = FDateTime::Now();
    FString DateTimeString = Now.ToString(TEXT("%Y-%m-%d %H:%M:%S"));

    //Main JSON to send the email (1 with all the txt files in the folder)
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField("token", "MI_TOKEN_SECRETO");
    JsonObject->SetStringField("subject", FString::Printf(TEXT("EVL - Summary of ALL data - %s"), *DateTimeString));
    JsonObject->SetArrayField("files", FileArray);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    FString Url = TEXT("https://script.google.com/macros/s/AKfycbzaQ4bubkdsYNXJuHvzZkxHsjPTlwAX9GYn7cLg-nknv1XKtZ0gsKAiSfiKzg9UEOxa/exec");

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &AHttpSenderActor::OnResponseReceived);
    Request->SetURL(Url);
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "application/json");
    Request->SetContentAsString(OutputString);

    Request->ProcessRequest();
    
    /*OLD
    //Send each file separately
    
    for (const FString& FileName : Files)
    {
        FString FilePath = Directory / FileName;
        SendTxtFile(FilePath);
    }
    */
}

//Send 1 .png image

void AHttpSenderActor::SendPngFile(const FString& FilePath)
{
    TArray<uint8> FileData;
    if (!FFileHelper::LoadFileToArray(FileData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("The PNG file could not be read: %s"), *FilePath);
        return;
    }

    //Turn into base64 to get sent
    FString Base64Content = FBase64::Encode(FileData);

    FString Url = TEXT("https://script.google.com/macros/s/AKfycbw-C1BO7_1osjGQHCdTjnkxtwOuuF4Yem8wS2dkDEm3V7qnTy7JbhfUAgrCB7NBU7Z1/exec");

    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField("token", "MI_TOKEN_SECRETO");
    JsonObject->SetStringField("filename", FPaths::GetCleanFilename(FilePath));
    JsonObject->SetStringField("content", Base64Content);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    //Create request
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &AHttpSenderActor::OnResponseReceived);
    Request->SetURL(Url);
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "application/json");
    Request->SetContentAsString(OutputString);

    Request->ProcessRequest();
}

//Send ALL .png images it can find in the folder Data/Photos

void AHttpSenderActor::SendAllPngFiles()
{
    FString Directory = FPaths::ProjectSavedDir() / TEXT("Data/Photos");

    IFileManager& FileManager = IFileManager::Get();
    TArray<FString> Files;
    FileManager.FindFiles(Files, *(Directory / TEXT("*.png")), true, false);

    if (Files.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("PNG files could not be found: %s"), *Directory);
        return;
    }

    //Send all the images can be found in the same mail
    TArray<TSharedPtr<FJsonValue>> FileArray;

    for (const FString& FileName : Files)
    {
        FString FilePath = Directory / FileName;
        TArray<uint8> FileData;
        if (FFileHelper::LoadFileToArray(FileData, *FilePath))
        {
            FString Base64Content = FBase64::Encode(FileData);

            TSharedPtr<FJsonObject> FileObject = MakeShareable(new FJsonObject);
            FileObject->SetStringField("filename", FileName);
            FileObject->SetStringField("content", Base64Content);
            FileArray.Add(MakeShareable(new FJsonValueObject(FileObject)));
        }
    }

    //Main JSON to send
    //Current date and time
    FDateTime Now = FDateTime::Now();
    FString DateTimeString = Now.ToString(TEXT("%Y-%m-%d %H:%M:%S"));

    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField("token", "MI_TOKEN_SECRETO");
    JsonObject->SetStringField("subject", FString::Printf(TEXT("EVL - ALL photos - %s"), *DateTimeString));
    JsonObject->SetArrayField("files", FileArray);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    FString Url = TEXT("https://script.google.com/macros/s/AKfycbzaQ4bubkdsYNXJuHvzZkxHsjPTlwAX9GYn7cLg-nknv1XKtZ0gsKAiSfiKzg9UEOxa/exec");

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &AHttpSenderActor::OnResponseReceived);
    Request->SetURL(Url);
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "application/json");
    Request->SetContentAsString(OutputString);

    Request->ProcessRequest();


    /*OLD 
    //Send all the .png images one-by-one
    for (const FString& FileName : Files)
    {
        FString FilePath = Directory / FileName;
        SendPngFile(FilePath);
    }*/
}

void AHttpSenderActor::SendPlayerSessionFiles(const FString& PlayerID, const FString& PlayerName)
{
    FString DirectoryTxt = FPaths::ProjectSavedDir() / TEXT("Data");
    FString DirectoryPng = FPaths::ProjectSavedDir() / TEXT("Data/Photos");

    IFileManager& FileManager = IFileManager::Get();

    //Search txt
    TArray<FString> TxtFiles;
    FileManager.FindFiles(TxtFiles, *(DirectoryTxt / TEXT("*.txt")), true, false);

    //Search png
    TArray<FString> PngFiles;
    FileManager.FindFiles(PngFiles, *(DirectoryPng / TEXT("*.png")), true, false);

    //Save files from the player
    TArray<TSharedPtr<FJsonValue>> FileArray;

    //Filter and add txt
    for (const FString& FileName : TxtFiles)
    {
        if (!FileName.EndsWith(PlayerID)) continue;

        FString FilePath = DirectoryTxt / FileName;
        FString FileContent;
        if (FFileHelper::LoadFileToString(FileContent, *FilePath))
        {
            TSharedPtr<FJsonObject> FileObject = MakeShareable(new FJsonObject);
            FileObject->SetStringField("filename", FileName);
            FileObject->SetStringField("content", FileContent);
            FileArray.Add(MakeShareable(new FJsonValueObject(FileObject)));
        }
    }

    //Filter and add png
    for (const FString& FileName : PngFiles)
    {
        if (!FileName.EndsWith(PlayerID)) continue;

        FString FilePath = DirectoryPng / FileName;
        TArray<uint8> FileData;
        if (FFileHelper::LoadFileToArray(FileData, *FilePath))
        {
            FString Base64Content = FBase64::Encode(FileData);

            TSharedPtr<FJsonObject> FileObject = MakeShareable(new FJsonObject);
            FileObject->SetStringField("filename", FileName);
            FileObject->SetStringField("content", Base64Content);
            FileArray.Add(MakeShareable(new FJsonValueObject(FileObject)));
        }
    }

    if (FileArray.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No se encontraron archivos para el jugador con ID: %s"), *PlayerID);
        return;
    }

    //Date and time
    FDateTime Now = FDateTime::Now();
    FString DateTimeString = Now.ToString(TEXT("%Y-%m-%d %H:%M:%S"));

    //JSON
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField("token", "MI_TOKEN_SECRETO");
    JsonObject->SetStringField("subject", FString::Printf(TEXT("EVL - Player files %s (%s) - %s"), *PlayerName, *PlayerID, *DateTimeString));
    JsonObject->SetArrayField("files", FileArray);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    FString Url = TEXT("https://script.google.com/macros/s/AKfycbzaQ4bubkdsYNXJuHvzZkxHsjPTlwAX9GYn7cLg-nknv1XKtZ0gsKAiSfiKzg9UEOxa/exec");

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &AHttpSenderActor::OnResponseReceived);
    Request->SetURL(Url);
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "application/json");
    Request->SetContentAsString(OutputString);

    Request->ProcessRequest();
}


//Debug done correctly (or not)

void AHttpSenderActor::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("Host response: %s"), *Response->GetContentAsString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Error in sending HTTP request"));
    }
}


