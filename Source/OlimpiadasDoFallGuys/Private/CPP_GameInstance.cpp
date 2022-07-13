// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GameInstance.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

UCPP_GameInstance::UCPP_GameInstance()
	:
	OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UCPP_GameInstance::OnCreateSessionComplete)),
	OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UCPP_GameInstance::OnFindSessionsComplete)),
	OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UCPP_GameInstance::OnJoinSessionComplete))
{

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

void UCPP_GameInstance::CreateGameSession()
{
	if (!OnlineSessionInterface.IsValid())
		return;

	if (const auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession); ExistingSession != nullptr)
		OnlineSessionInterface->DestroySession(NAME_GameSession);

	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

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

void UCPP_GameInstance::ListGameSessions()
{
	if (!OnlineSessionInterface.IsValid())
		return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void UCPP_GameInstance::JoinGameSession(FString SessionId)
{
	if (!OnlineSessionInterface.IsValid())
		return;

	for (const auto Result : SessionSearch->SearchResults)
	{
		if (Result.GetSessionIdStr() != SessionId)
			continue;

		const FString Id = Result.GetSessionIdStr();
		const FString Owner = Result.Session.OwningUserName;
		FString MatchType;

		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);
		
		if (MatchType != FString("FreeForAll"))
			return;

		OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
		
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, .15f, FColor::Blue, FString::Printf(L"Joining %s session (Id: %s, Host: %s", *MatchType, *Id, *Owner));

		const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

		OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
		break;
	}
}

void UCPP_GameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful) const
{
	if (bWasSuccessful)
	{
		OnGameSessionCreatedDelegate.Broadcast(SessionName, bWasSuccessful);

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Session %s created successfully."), *SessionName.ToString()));
	}
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("Failed to create a new session.")));
	}
}

void UCPP_GameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("Failed to load sessions.")));
	}

	TArray<FOnlineGameSession> OnlineGameSessions;
	
	for (const auto SearchResult : SessionSearch->SearchResults)
	{
		FOnlineGameSession GameSession = {
			SearchResult.GetSessionIdStr(),
			SearchResult.Session.OwningUserName,
			SearchResult.Session.SessionSettings.NumPublicConnections,
			SearchResult.Session.SessionSettings.NumPublicConnections - SearchResult.Session.NumOpenPublicConnections,
			SearchResult.PingInMs
		};

		OnlineGameSessions.Add(GameSession);
	}


	const FOnlineGameSessionResult Result {
		OnlineGameSessions,
	};

	OnGameSessionsFoundDelegate.Broadcast(bWasSuccessful, Result);
}

void UCPP_GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!OnlineSessionInterface.IsValid())
		return;

	FString Address;

	if (OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(L"Connected to session at %s", *Address));

		if (APlayerController* PlayerController = GetFirstLocalPlayerController(); PlayerController != nullptr)
			PlayerController->ClientTravel(Address, TRAVEL_Absolute);
	}
}

