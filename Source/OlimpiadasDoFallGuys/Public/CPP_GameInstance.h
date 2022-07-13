// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "CPP_GameInstance.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FOnlineGameSession
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Online Session Data")
	FString Id;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Online Session Data")
	FString SessionName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Online Session Data")
	int MaxPlayers;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Online Session Data")
	int CurrentPlayers;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Online Session Data")
	int Ping;
};

USTRUCT(BlueprintType)
struct FOnlineGameSessionResult
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Online Session Data")
	TArray<FOnlineGameSession> Sessions;
};

UCLASS()
class OLIMPIADASDOFALLGUYS_API UCPP_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UCPP_GameInstance();
	
	UFUNCTION(BlueprintCallable)
	void CreateGameSession();
	
	UFUNCTION(BlueprintCallable)
	void ListGameSessions();
	
	UFUNCTION(BlueprintCallable)
	void JoinGameSession(FString SessionId);


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameSessionCreatedDelegate, FName, SessionName, bool, bWasSuccesful);
	UPROPERTY(BlueprintAssignable, Category="Online Game Session")
	FOnGameSessionCreatedDelegate OnGameSessionCreatedDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameSessionsFoundDelegate, bool, bWasSuccessful, FOnlineGameSessionResult, Result);
	UPROPERTY(BlueprintAssignable, Category="Online Game Session")
	FOnGameSessionsFoundDelegate OnGameSessionsFoundDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameSessionJoinedDelegate, FString, SessionId, bool, bWasSuccessful);
	UPROPERTY(BlueprintAssignable, Category="Online Game Session")
	FOnGameSessionJoinedDelegate OnGameSessionJoinedDelegate;

private:
	TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> OnlineSessionInterface;

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful) const;
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
};
