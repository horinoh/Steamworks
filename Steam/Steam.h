#pragma once

#include "resource.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <random>

#pragma warning(disable:4819)
#pragma warning(disable:4996)
#include "steam/steam_api.h"
#include "steam/steam_gameserver.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if(nullptr != x) { delete x; x = nullptr; }
#endif
#ifndef SAFE_FCLOSE
#define SAFE_FCLOSE(x) if(nullptr != x) { fclose(x); x = nullptr; }
#endif

class Steam
{
public:
	Steam();
	virtual ~Steam();

	virtual void OnTimer(HWND hWnd, HINSTANCE hInstance);

	void StartServer();
	void StartClient();
	
	void StartController();

	class GameServer* GetGameServer() { return mpGameServer; }
	class GameClient* GetGameClient() { return mpGameClient; }

	static void PrintUsage();
	void HandleInput(WPARAM wParam);

protected:
	FILE* StdOut = nullptr;
	FILE* StdErr = nullptr;

	class GameServer* mpGameServer = nullptr;
	class GameClient* mpGameClient = nullptr;

	ControllerActionSetHandle_t mInGameActionSet;
	ControllerActionSetHandle_t mMenuActionSet;

	ControllerAnalogActionHandle_t mMoveAction;
	ControllerDigitalActionHandle_t mFireAction;

	ControllerDigitalActionHandle_t mMenuUpAction;
	ControllerDigitalActionHandle_t mMenuDownAction;
	ControllerDigitalActionHandle_t mMenuSelectAction;
	ControllerDigitalActionHandle_t mMenuCancelAction;
};

class GameServer
{
public:
	GameServer();
	virtual ~GameServer();

	enum class EServerState
	{
		None,
	};

	virtual void OnTimer(HWND hWnd, HINSTANCE hInstance);
	virtual void ReceiveData();

	STEAM_GAMESERVER_CALLBACK(GameServer, OnValidateAuthTicketResponse, ValidateAuthTicketResponse_t);

	STEAM_GAMESERVER_CALLBACK(GameServer, OnSteamServersConnected, SteamServersConnected_t);
	STEAM_GAMESERVER_CALLBACK(GameServer, OnSteamServerConnectFailure, SteamServerConnectFailure_t);
	STEAM_GAMESERVER_CALLBACK(GameServer, OnSteamServersDisconnected, SteamServersDisconnected_t);

protected:
	EServerState mState = EServerState::None;
};

class GameClient
{
public:
	GameClient();
	virtual ~GameClient();

	enum class EClientState
	{
		None,
		InLobby,
		CreatingLobby,
		FindingLobby,
		JoiningLobby,
		Connecting,
	};

	virtual void OnTimer(HWND hWnd, HINSTANCE hInstance);
	virtual void ReceiveData();

	STEAM_CALLBACK(GameClient, OnSteamServersConnected, SteamServersConnected_t);
	STEAM_CALLBACK(GameClient, OnSteamServersDisconnected, SteamServersDisconnected_t);
	STEAM_CALLBACK(GameClient, OnSteamServerConnectFailure, SteamServerConnectFailure_t);

	void OnLobbyCreated(LobbyCreated_t *pCallback, bool bIOFailure);
	void OnLobbyMatchListed(LobbyMatchList_t* pCallback, bool bIOFailure);
	void OnLobbyEntered(LobbyEnter_t *pCallback, bool bIOFailure);

	STEAM_CALLBACK(GameClient, OnLobbyGameCreated, LobbyGameCreated_t);
	STEAM_CALLBACK(GameClient, OnLobbyChatMsg, LobbyChatMsg_t);

	void CreateLobby();
	void FindLobby();
	void JoinLobby(const uint32 Index);
	void LeaveLobby();

	static void PrintLobbyStatus(const CSteamID LobbySteamID);
	void ToggleReady();

	STEAM_CALLBACK(GameClient, OnUserStatsReceived, UserStatsReceived_t);
	STEAM_CALLBACK(GameClient, OnUserStatsStored, UserStatsStored_t);
	STEAM_CALLBACK(GameClient, OnUserAchievementStored, UserAchievementStored_t);
	void StoreStats();
	void ResetStats();

	void OnLeaderboardFindResult(LeaderboardFindResult_t *pFindLearderboardResult, bool bIOFailure);
	void OnLeaderboardScoresDownloaded(LeaderboardScoresDownloaded_t *pLeaderboardScoresDownloaded, bool bIOFailure);
	void OnLeaderboardScoreUploaded(LeaderboardScoreUploaded_t *pFindLearderboardResult, bool bIOFailure);
	void FindOrCreateLeaderboard();
	void UploadLeaderboard();

	CSteamID GetEnteredLobbySteamID() const { return mEnteredLobbySteamID; }

protected:
	CCallResult<GameClient, LobbyCreated_t> mOnLobbyCreated;
	CCallResult<GameClient, LobbyMatchList_t> mOnLobbyMatchListed;
	CCallResult<GameClient, LobbyEnter_t> mOnLobbyEntered;

	CSteamID mEnteredLobbySteamID = CSteamID();
	std::vector<CSteamID> mFoundLoobySteamID;

	HAuthTicket mAuthTicket = k_HAuthTicketInvalid;

	EClientState mState = EClientState::None;

	int32 mNumGames = 0, mNumWins = 0, mNumLosses = 0;
	float mFeetTraveled = 0.0f, mMaxFeetTraveled = 0.0f;

	SteamLeaderboard_t mSteamLeaderboard;
	CCallResult<GameClient, LeaderboardFindResult_t> mLeaderboardFindResult;
	CCallResult<GameClient, LeaderboardScoresDownloaded_t> mLeaderboardScoresDownloaded;
	CCallResult<GameClient, LeaderboardScoreUploaded_t> mLeaderboardScoreUploaded;
};