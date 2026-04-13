#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Http.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpSenderActor.generated.h"

//Default values
UCLASS()
class THEVIRTUALCODE_API AHttpSenderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AHttpSenderActor();

	//Send .txt files
	UFUNCTION(BlueprintCallable, Category = "HTTP")
	void SendTxtFile(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "HTTP")
	void SendAllTxtFiles();

	//Send .png files
	UFUNCTION(BlueprintCallable, Category = "HTTP")
	void SendPngFile(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "HTTP")
	void SendAllPngFiles();

	//Send files from a specific player
	UFUNCTION(BlueprintCallable, Category = "HTTP")
	void SendPlayerSessionFiles(const FString& PlayerID, const FString& PlayerName);


protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
private:
	//Was the submission successful?
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);


};
