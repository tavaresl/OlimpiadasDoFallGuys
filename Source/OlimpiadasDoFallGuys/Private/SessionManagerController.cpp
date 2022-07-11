// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionManagerController.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

// Sets default values
ASessionManagerController::ASessionManagerController()
	:
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ASessionManagerController::OnCreateSessionComplete))
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

// Called when the game starts or when spawned
void ASessionManagerController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASessionManagerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASessionManagerController::CreateGameSession() const
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

void ASessionManagerController::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
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

