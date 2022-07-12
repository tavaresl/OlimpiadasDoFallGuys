// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "SessionController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSessionCreatedDelegate, FName, SessionName);
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

protected:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION()
	void HandleGameSessionCreatedEvent() const;
	virtual void NotifyGameSessionCreated();
	
	UFUNCTION(BlueprintCallable)
	void CreateGameSession() const;
	
	//UFUNCTION(BlueprintCallable)
	//void JoinGameSession();
	
	//UFUNCTION(BlueprintCallable)
	//void ListGameSessions();

private:
	FName CreatedSessionName;
	TSharedPtr<FOnlineSessionSearchResult> SessionSearchResult;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	//FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	//FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;

	UPROPERTY(BlueprintAssignable, Category="GameSession")
	FOnGameSessionCreatedDelegate OnGameSessionCreatedDelegate;
};
