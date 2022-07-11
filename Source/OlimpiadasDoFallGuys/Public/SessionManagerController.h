// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "SessionManagerController.generated.h"

UCLASS()
class OLIMPIADASDOFALLGUYS_API ASessionManagerController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASessionManagerController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> OnlineSessionInterface;

protected:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	
	UFUNCTION(BlueprintCallable)
	void CreateGameSession() const;
	
	//UFUNCTION(BlueprintCallable)
	//void JoinGameSession();
	
	//UFUNCTION(BlueprintCallable)
	//void ListGameSessions();

	private:
	TSharedPtr<FOnlineSessionSearchResult> SessionSearchResult;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	//FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	//FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;


};
