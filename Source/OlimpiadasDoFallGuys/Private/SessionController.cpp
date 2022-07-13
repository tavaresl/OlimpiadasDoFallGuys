// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionController.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

// Sets default values for this component's properties
USessionController::USessionController()
	:
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &USessionController::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &USessionController::OnFindSessionsComplete))
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Online subsystem %s found."), *OnlineSubsystem->GetSubsystemName().ToString()));
		}
	}
	else
	{

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Cannot find any online subsystem.")));
		}
	}
}


// Called when the game starts
void USessionController::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USessionController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USessionController::CreateGameSession() const
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	if (const auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession); ExistingSession != nullptr)
		OnlineSessionInterface->DestroySession(NAME_GameSession);

	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	const TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());

	SessionSettings->bIsLANMatch = false;
	SessionSettings->NumPublicConnections = 4;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUseLobbiesIfAvailable = true;
	SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineService);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
}

void USessionController::ListGameSessions()
{
	if (!OnlineSessionInterface.IsValid())
		return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch);

	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void USessionController::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			CreatedSessionName = SessionName;
			NotifyGameSessionCreated();
			GEngine->AddOnScreenDebugMessage(-1,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Session %s created successfully."), *SessionName.ToString()));
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,
				15.f,
				FColor::Red,
				FString::Printf(TEXT("Failed to create a new session.")));
		}
	}
}

void USessionController::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (!OnlineSessionInterface.IsValid())
		return;

	if (SessionSearch->SearchResults.Num() == 0)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(L"Cannot find any online session"));

		return;
	}

	for (FOnlineSessionSearchResult Result : SessionSearch->SearchResults)
	{
		FString MatchType;

		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);
		
		FSearchResult_OLD SearchResult = {
			Result.GetSessionIdStr(),
			Result.Session.OwningUserName,
			Result
		};

		if (MatchType == FString("FreeForAll"))
		{
			SessionSearchResults.Add(SearchResult);
			NotifyGameSessionFound(SearchResult.Id);
		}
	}

	NotifyGameSessionsFound(bWasSuccessful);
}

void USessionController::HandleGameSessionCreatedEvent() const
{
	OnGameSessionCreatedDelegate.Broadcast(CreatedSessionName);
}

void USessionController::NotifyGameSessionCreated()
{
	HandleGameSessionCreatedEvent();
}

void USessionController::HandleGameSessionsFoundEvent(bool bWasSuccessful) const
{
	OnGameSessionsFoundDelegate.Broadcast(bWasSuccessful);
}

void USessionController::NotifyGameSessionsFound(bool bWasSuccessful)
{
	HandleGameSessionCreatedEvent();
}

void USessionController::HandleGameSessionFoundEvent(FString SessionId) const
{
	OnGameSessionFoundDelegate.Broadcast(SessionId);
}

void USessionController::NotifyGameSessionFound(FString SessionId)
{
	HandleGameSessionFoundEvent(SessionId);
}

