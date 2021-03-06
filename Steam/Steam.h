#pragma once

#include "resource.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <array>
#include <random>
#include <algorithm>

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

#ifndef USE_DX
#define USE_DX
#endif

#ifdef USE_DX
#include <wrl.h>
#include <d3d12.h>
#include <DXGI1_4.h>
#include <DirectXColors.h>

#ifndef VERIFY_SUCCEEDED
#define VERIFY_SUCCEEDED(vr) if(FAILED(vr)) { DebugBreak(); }
#endif
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

	void OnLeaderboardFindResultAndUpload(LeaderboardFindResult_t *pCallback, bool bIOFailure);
	void OnLeaderboardFindResultAndDownload(LeaderboardFindResult_t *pCallback, bool bIOFailure);
	void OnLeaderboardScoresDownloaded(LeaderboardScoresDownloaded_t *pCallback, bool bIOFailure);
	void OnRemoteStorageDownloadUGCResult(RemoteStorageDownloadUGCResult_t *pCallback, bool bIOFailure);
	void OnLeaderboardScoreUploaded(LeaderboardScoreUploaded_t *pCallback, bool bIOFailure);
	void OnRemoteStorageFileShareResult(RemoteStorageFileShareResult_t *pCallback, bool bIOFailure);
	void OnLeaderboardUGCSet(LeaderboardUGCSet_t *pCallback, bool bIOFailure);
	void UploadLeaderboard(const char* LeaderboardName);
	void DownloadLeaderboard(const char* LeaderboardName);

	STEAM_CALLBACK(GameClient, OnDlcInstalled, DlcInstalled_t);
	void QueryDLC();

	void OnCreateItemResult(CreateItemResult_t *pCallback, bool bIOFailure);
	void OnSubmitItemUpdateResult(SubmitItemUpdateResult_t* pCallback, bool bIOFailure);
	void OnRemoteStorageSubscribePublishedFileResult(RemoteStorageSubscribePublishedFileResult_t *pCallback, bool bIOFailure);
	void OnRemoteStorageUnsubscribePublishedFileResult(RemoteStorageUnsubscribePublishedFileResult_t *pCallback, bool bIOFailure);
	void OnSteamUGCQueryCompleted(SteamUGCQueryCompleted_t *pCallback, bool bIOFailure);
	STEAM_CALLBACK(GameClient, OnRemoteStoragePublishedFileSubscribed, RemoteStoragePublishedFileSubscribed_t);
	STEAM_CALLBACK(GameClient, OnRemoteStoragePublishedFileUnsubscribed, RemoteStoragePublishedFileUnsubscribed_t);
	//STEAM_CALLBACK(GameClient, OnRemoteStorageEnumerateUserPublishedFilesResult, RemoteStorageEnumerateUserPublishedFilesResult_t);
	//STEAM_CALLBACK(GameClient, OnRemoteStorageEnumerateUserSubscribedFilesResult, RemoteStorageEnumerateUserSubscribedFilesResult_t);
	//STEAM_CALLBACK(GameClient, OnRemoteStorageEnumerateWorkshopFilesResult, RemoteStorageEnumerateWorkshopFilesResult_t);
	//STEAM_CALLBACK(GameClient, OnRemoteStorageEnumerateUserSharedWorkshopFilesResult, RemoteStorageEnumerateUserSharedWorkshopFilesResult_t);
	//STEAM_CALLBACK(GameClient, OnRemoteStorageEnumeratePublishedFilesByUserActionResult, RemoteStorageEnumeratePublishedFilesByUserActionResult_t);
	STEAM_CALLBACK(GameClient, OnItemInstalled, ItemInstalled_t);
	void CreateUGC();
	void QueryUGC();

	void OnRemoteStorageFileReadAsyncComplete(RemoteStorageFileReadAsyncComplete_t *pCallback, bool bIOFailure);
	void OnRemoteStorageFileWriteAsyncComplete(RemoteStorageFileWriteAsyncComplete_t *pCallback, bool bIOFailure);
	void QueryRemoteStoage();
	void WriteRemoteStorage();
	void ReadRemoteStorage();

	CSteamID GetEnteredLobbySteamID() const { return mEnteredLobbySteamID; }

	void GetFriendInformation();

	void PrintFriendRelationship();
	void PrintPersonaName(const CSteamID SteamID);
	void PrintNickname(const CSteamID SteamID);
	void PrintSteamLevel(const CSteamID SteamID);
	void PrintPersonaState(const CSteamID SteamID);
	void PrintGamePlayed(const CSteamID SteamID);
	void PrintAvatarInfo(const CSteamID SteamID);
	void OnImageLoad(const int ImageHandle);
	STEAM_CALLBACK(GameClient, OnPersonaStateChange, PersonaStateChange_t);
	STEAM_CALLBACK(GameClient, OnAvatarImageLoaded, AvatarImageLoaded_t);
	STEAM_CALLBACK(GameClient, OnGameOverlayActivated, GameOverlayActivated_t);

	//GameServerChangeRequested_t;
	//GameLobbyJoinRequested_t;
	//ClanOfficerListResponse_t;
	//FriendRichPresenceUpdate_t;
	//GameRichPresenceJoinRequested_t;
	//GameConnectedClanChatMsg_t;
	//GameConnectedChatJoin_t;
	//GameConnectedChatLeave_t;
	//DownloadClanActivityCountsResult_t;
	//JoinClanChatRoomCompletionResult_t;
	//GameConnectedFriendChatMsg_t;
	//FriendsGetFollowerCount_t;
	//FriendsIsFollowing_t;
	//FriendsEnumerateFollowingList_t;
	//SetPersonaNameResponse_t;

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
	CCallResult<GameClient, RemoteStorageDownloadUGCResult_t> mRemoteStorageDownloadUGCResult;
	CCallResult<GameClient, LeaderboardScoreUploaded_t> mLeaderboardScoreUploaded;
	CCallResult<GameClient, RemoteStorageFileShareResult_t> mRemoteStorageFileShareResult;
	CCallResult<GameClient, LeaderboardUGCSet_t> mLeaderboardUGCSet;

	CCallResult<GameClient, CreateItemResult_t> mCreateItemResult;
	CCallResult<GameClient, SubmitItemUpdateResult_t> mSubmitItemUpdateResult;
	CCallResult<GameClient, RemoteStorageSubscribePublishedFileResult_t> mRemoteStorageSubscribePublishedFileResult;
	CCallResult<GameClient, RemoteStorageUnsubscribePublishedFileResult_t> mRemoteStorageUnsubscribePublishedFileResult;
	CCallResult<GameClient, SteamUGCQueryCompleted_t> mSteamUGCQueryCompleted;

	CCallResult<GameClient, RemoteStorageFileReadAsyncComplete_t> mRemoteStorageFileReadAsyncComplete;
	CCallResult<GameClient, RemoteStorageFileWriteAsyncComplete_t> mRemoteStorageFileWriteAsyncComplete;
	std::string mRemoteStorageFile = "Savedata";
	std::vector<uint8> mRemoteStorageBuffer = { 0 };

	PublishedFileId_t mPublishedFileId = 0;
};

#ifdef USE_DX
class DX
{
public:
	void OnCreate(HWND hWnd);
	void OnSize(HWND hWnd, HINSTANCE hInstance);
	void OnPaint(HWND hWnd, HINSTANCE hInstance);
	void OnDestroy(HWND hWnd, HINSTANCE hInstance);

	void CreateDevice(HWND hWnd);
	void CreateCommandQueue();
	void CreateFence();
	void CreateSwapchain(HWND hWnd);
	void CreateCommandList();
	void CreateSwapChainResource();
	void CreateRootSignature();
	void CreatePipelineState();
	void CreateViewport(const FLOAT Width, const FLOAT Height, const FLOAT MinDepth = 0.0f, const FLOAT MaxDepth = 1.0f);

	void ResourceBarrier(ID3D12GraphicsCommandList* CommandList, ID3D12Resource* Resource, const D3D12_RESOURCE_STATES Before, const D3D12_RESOURCE_STATES After);
	void PopulateCommandList(const size_t i);

	void Draw();
	void WaitForFence();

protected:
	Microsoft::WRL::ComPtr<ID3D12Device> Device;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue;

	Microsoft::WRL::ComPtr<ID3D12Fence> Fence;
	UINT64 FenceValue = 0;

	Microsoft::WRL::ComPtr<IDXGISwapChain3> SwapChain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SwapChainDescriptorHeap;
	UINT CurrentBackBufferIndex = 0xffffffff;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> SwapChainResources;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator;
	std::vector<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> GraphicsCommandLists;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignature;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineState;

	std::vector<D3D12_VIEWPORT> Viewports;
	std::vector<D3D12_RECT> ScissorRects;
};
#endif