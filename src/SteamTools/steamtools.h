#ifndef STEAMTOOLS_WRAPPER
#define STEAMTOOLS_WRAPPER

#include "steam/steam_api.h"
#include <string>
#include <iostream>
#include <vector>
#include "JSON/json.hpp"

#define PACKET_DELIMTER  " "
#define DATA_EDGE '\"'
#define PACKET_END "ENDDATA"

namespace SteamTools {
	enum NetworkStatus { offline, searching, creating, joining, connected };
	enum NetworkPosition {local, lobby, host,remote};
	enum DataType {UNINIT,NULLDATA};

	struct Message {
		DataType type = DataType::UNINIT;
		int length = -1;
		std::string data = "";
	};

	class SteamManager {
	public:
		SteamManager();
		static bool Init(int appid = 480);
		static void ProcessCallbacks();

		// ----------STEAM CLIENT----------
		static std::string GetMySteamName();
		auto isOverlayActive() { return overlay_active; };

		// ----------NETWORKING----------

		// MATCHAMKING
		void FindLobbies();
		auto GetLobbiesFound() { return found_lobbies; };
		CSteamID JoinLobbyByIdx(int idx) { CSteamID lobby = SteamMatchmaking()->GetLobbyByIndex(idx); SteamMatchmaking()->JoinLobby(lobby); return lobby; };
		auto GetLobbyConnectionSuccess() { return lobby_connection_success; };
		void CreateLobby(int max_players, ELobbyType type = ELobbyType::k_ELobbyTypePublic) { SteamMatchmaking()->CreateLobby(type, max_players); created_lobby_id = static_cast<uint64>(-1); };
		auto GetCreatedLobbyId() { return created_lobby_id; };
		auto GetCurrentLobby() { return current_lobby; };
		auto GetNetworkStatus() { return network_status; };
		auto GetNetworkPosition() { return network_position; };
		std::vector<std::string> GetLobbyMemberNames();

		void MatchmakeLoop_Quick(int new_lobby_max_players);

		// CONNECTION
		void HostP2P(int channel = 0);
		void ConnectP2P(CSteamID remoteID,int channel = 0);
		SteamNetworkingIdentity IdentityFromSteamID(CSteamID id);
		auto GetRemotes() { return remotes; };
		bool SendMessageP2P_Reliable(int remote_idx, const char* data,int channel = 0);
		bool SendMessageP2P_Reliable(SteamNetworkingIdentity remote, const char* data, int channel = 0);
		bool SendMessageP2P_Unreliable(int remote_idx, const char* data, int channel = 0);
		bool SendMessageP2P_Unreliable(SteamNetworkingIdentity remote, const char* data, int channel = 0);
		void ReceiveMessageP2P(int channel = 0);
		std::string PreparePayload(const char* data, DataType type = NULLDATA);
		Message ParsePayload(std::string data);
	private:
		STEAM_CALLBACK(SteamManager, OnGameOverlayActivated, GameOverlayActivated_t);
		bool overlay_active = false;
		
		NetworkStatus network_status = NetworkStatus::offline; NetworkPosition network_position = NetworkPosition::local; CSteamID current_lobby;
		void OnLobbyListFound(LobbyMatchList_t* pLobbyMatchList, bool bIOFailure);
		CCallResult<SteamManager, LobbyMatchList_t> m_LobbyMatchList_t;
		int found_lobbies = -1;
		STEAM_CALLBACK(SteamManager, OnLobbyEnter, LobbyEnter_t);
		bool lobby_connection_success = false;
		STEAM_CALLBACK(SteamManager, OnLobbyCreated, LobbyCreated_t);
		CSteamID created_lobby_id = static_cast<uint64>(-1);
		STEAM_CALLBACK(SteamManager, OnGameLobbyJoinRequested_t, GameLobbyJoinRequested_t);
		STEAM_CALLBACK(SteamManager, OnSteamNetConnectionStatusChangedCallback_t, SteamNetConnectionStatusChangedCallback_t);
		STEAM_CALLBACK(SteamManager, OnSteamNetworkingMessagesSessionRequest_t, SteamNetworkingMessagesSessionRequest_t);
		std::vector<SteamNetworkingIdentity> remotes;
		std::vector<std::string> received_messages;
	};
}

#endif