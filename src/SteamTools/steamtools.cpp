#include "steamtools.h"

SteamTools::SteamManager::SteamManager() {}

bool SteamTools::SteamManager::Init(int appid)
{
	if (SteamAPI_RestartAppIfNecessary(appid))
		return false;
	SteamAPI_Init();
	return true;
}

std::string SteamTools::SteamManager::GetMySteamName()
{
	return SteamFriends()->GetPersonaName();
}

void SteamTools::SteamManager::ProcessCallbacks()
{
	SteamAPI_RunCallbacks();
}

void SteamTools::SteamManager::OnGameOverlayActivated(GameOverlayActivated_t* pCallback)
{
	this->overlay_active = pCallback->m_bActive;
}

void SteamTools::SteamManager::FindLobbies()
{
    if (network_status != NetworkStatus::offline) {
        SteamMatchmaking()->LeaveLobby(current_lobby);
        network_status = NetworkStatus::offline;
    }
    network_status = NetworkStatus::searching;
    m_LobbyMatchList_t.Set(SteamMatchmaking()->RequestLobbyList(), this, &SteamManager::OnLobbyListFound);
    this->found_lobbies = -1;
}

void SteamTools::SteamManager::OnLobbyListFound(LobbyMatchList_t* pLobbyMatchList, bool bIOFailure) {
	this->found_lobbies = pLobbyMatchList->m_nLobbiesMatching;
}

void SteamTools::SteamManager::OnLobbyEnter(LobbyEnter_t* pCallback)
{
	this->lobby_connection_success = pCallback->m_EChatRoomEnterResponse == k_EChatRoomEnterResponseSuccess;
    current_lobby = pCallback->m_ulSteamIDLobby;

    network_status = NetworkStatus::connected;
    SteamFriends()->SetRichPresence("connect", std::to_string(current_lobby.GetAccountID()).c_str());
    std::cout << "SET \"connect\" to " << std::to_string(current_lobby.GetAccountID()).c_str() << "\n";
}

void SteamTools::SteamManager::OnLobbyCreated(LobbyCreated_t* pCallback)
{
	this->created_lobby_id = pCallback->m_ulSteamIDLobby;
    current_lobby = created_lobby_id;

    SteamMatchmaking()->SetLobbyData(current_lobby, "BloomGameName", "Shooter");
    std::cout << "SET " << current_lobby.GetAccountID() << "\n";
    network_status = NetworkStatus::connected;
}

void SteamTools::SteamManager::OnGameLobbyJoinRequested_t(GameLobbyJoinRequested_t* pCallback)
{
    SteamMatchmaking()->JoinLobby(pCallback->m_steamIDLobby);
    network_status = NetworkStatus::connected;
}



std::vector<std::string> SteamTools::SteamManager::GetLobbyMemberNames()
{
    std::vector<std::string> names;
    for (int i = 0; i < SteamMatchmaking()->GetNumLobbyMembers(GetCurrentLobby()); i++)
    {
        names.push_back(SteamFriends()->GetFriendPersonaName(SteamMatchmaking()->GetLobbyMemberByIndex(GetCurrentLobby(), i)));
    }
    return names;
}

void SteamTools::SteamManager::Matchmake(int new_lobby_max_players) {
    switch (network_status) {
    case SteamTools::NetworkStatus::offline:
        break;
    case SteamTools::NetworkStatus::searching:
        if (GetLobbiesFound() != -1) {
            if (GetLobbiesFound() == 0) {
            } else {
                for (int i = 0; i < GetLobbiesFound() && network_status == SteamTools::NetworkStatus::searching; i++) {
                    std::string bloom_game_name = SteamMatchmaking()->GetLobbyData(SteamMatchmaking()->GetLobbyByIndex(i), "BloomGameName");
                    if (bloom_game_name == "Shooter") {
                        network_status = NetworkStatus::joining;
                        std::cout << "JOINING " << SteamMatchmaking()->GetLobbyByIndex(i).GetAccountID() << "\n";
                        JoinLobbyByIdx(i);
                    }
                }
                if (network_status == SteamTools::NetworkStatus::searching) {
                    CreateLobby(new_lobby_max_players);
                    network_status = NetworkStatus::creating;
                }
            }
        }
        break;
    case SteamTools::NetworkStatus::joining:
        break;
    case SteamTools::NetworkStatus::creating:
        break;
    case SteamTools::NetworkStatus::connected:
        break;
    }
}