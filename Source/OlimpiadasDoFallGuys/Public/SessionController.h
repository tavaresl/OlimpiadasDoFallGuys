// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Components/ActorComponent.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "SessionController.generated.h"

USTRUCT()
struct FSearchResult_OLD
{
	GENERATED_BODY();
	
	FString Id;
	FString Owner;
	FOnlineSessionSearchResult Session;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSessionCreatedDelegate, FName, SessionName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSessionsFoundDelegate, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSessionFoundDelegate, FString, SessionId);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OLIMPIADASDOFALLGUYS_API USessionController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USessionController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> OnlineSessionInterface;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GameSession")
	TArray<FString> FoundSessionsIds;

protected:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);

	UFUNCTION()
	void HandleGameSessionCreatedEvent() const;
	virtual void NotifyGameSessionCreated();

	UFUNCTION()
	void HandleGameSessionsFoundEvent(bool bWasSuccessful) const;
	virtual void NotifyGameSessionsFound(bool bWasSuccessful);

	UFUNCTION()
	void HandleGameSessionFoundEvent(FString SessionId) const;
	virtual void NotifyGameSessionFound(FString SessionId);
	
	UFUNCTION(BlueprintCallable)
	void CreateGameSession() const;
	
	UFUNCTION(BlueprintCallable)
	void ListGameSessions();
	
	//UFUNCTION(BlueprintCallable)
	//void JoinGameSession();

private:
	FName CreatedSessionName;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	TArray<FSearchResult_OLD> SessionSearchResults;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	//FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;

	UPROPERTY(BlueprintAssignable, Category="GameSession")
	FOnGameSessionCreatedDelegate OnGameSessionCreatedDelegate;

	UPROPERTY(BlueprintAssignable, Category="GameSession")
	FOnGameSessionsFoundDelegate OnGameSessionsFoundDelegate;

	UPROPERTY(BlueprintAssignable, Category="GameSession")
	FOnGameSessionFoundDelegate OnGameSessionFoundDelegate;
};