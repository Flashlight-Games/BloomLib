#ifndef STEAMTOOLS_WRAPPER
#define STEAMTOOLS_WRAPPER

#include "steam/steam_api.h"
#include <string>
#include <iostream>

namespace SteamTools {
	class SteamManager {
	public:
		SteamManager();
		static bool Init(int appid = 480);
		static void ProcessCallbacks();

		// STEAM CLIENT
		static std::string GetMySteamName();
		auto isOverlayActive() { return overlay_active; };

		// NETWORKING
		void FindLobbies();
		auto GetLobbiesFound() { return found_lobbies; };
		CSteamID JoinLobbyByIdx(int idx) { CSteamID lobby = SteamMatchmaking()->GetLobbyByIndex(idx); SteamMatchmaking()->JoinLobby(lobby); return lobby; };
		auto GetLobbyConnectionSuccess() { return lobby_connection_success; };
		void CreateLobby(int max_players, ELobbyType type = ELobbyType::k_ELobbyTypePublic) { SteamMatchmaking()->CreateLobby(type, max_players); created_lobby_id = static_cast<uint64>(-1); };
		auto GetCreatedLobbyId() { return created_lobby_id; };
	private:
		STEAM_CALLBACK(SteamManager, OnGameOverlayActivated, GameOverlayActivated_t);
		bool overlay_active = false;
		

		void OnLobbyListFound(LobbyMatchList_t* pLobbyMatchList, bool bIOFailure);
		CCallResult<SteamManager, LobbyMatchList_t> m_LobbyMatchList_t;
		int found_lobbies = -1;
		STEAM_CALLBACK(SteamManager, OnLobbyEnter, LobbyEnter_t);
		bool lobby_connection_success = false;
		STEAM_CALLBACK(SteamManager, OnLobbyCreated, LobbyCreated_t);
		CSteamID created_lobby_id = static_cast<uint64>(-1);
	};

	enum NetworkStatus { offline, searching, creating, joining, connected};
}

#endif